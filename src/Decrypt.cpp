// Decryption of password-protected xlsx files, as specified in [MS-OFFCRYPTO].
//
// An encrypted xlsx is an OLE2 compound file (CFB) holding two streams:
// `EncryptionInfo`, which describes how the key is derived from the password,
// and `EncryptedPackage`, which is the AES-encrypted xlsx zip. We support both
// variants of ECMA-376 encryption:
//
// * Standard (Office 2007): SHA-1 key derivation, AES-ECB.
// * Agile (Office 2010 and later): SHA-1/SHA-2 key derivation, AES-CBC.

#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "cpp11/protect.hpp"
#include "cpp11/raws.hpp"
#include "cpp11/sexp.hpp"
#include "cpp11/strings.hpp"

#include "libxls/xls.h"
#include "rapidxml/rapidxml.h"
#include "sha.h"

extern "C" {
void readxl_aes128_decrypt(const uint8_t*, const uint8_t*, uint8_t*, size_t);
void readxl_aes192_decrypt(const uint8_t*, const uint8_t*, uint8_t*, size_t);
void readxl_aes256_decrypt(const uint8_t*, const uint8_t*, uint8_t*, size_t);
}

namespace {

uint32_t le32(const std::string& s, size_t pos) {
  if (pos + 4 > s.size()) {
    throw std::runtime_error("Unexpected end of data in encrypted file.");
  }
  uint32_t x = 0;
  for (int i = 3; i >= 0; i--) {
    x = (x << 8) | static_cast<uint8_t>(s[pos + i]);
  }
  return x;
}

std::string le32_bytes(uint32_t x) {
  std::string out(4, '\0');
  for (int i = 0; i < 4; i++) {
    out[i] = static_cast<char>((x >> (8 * i)) & 0xff);
  }
  return out;
}

std::string to_string(const cpp11::raws& x) {
  return std::string(reinterpret_cast<const char*>(RAW(x)), x.size());
}

// Compound File Binary format ([MS-CFB]), read with libxls --------------------
//
// ole2_open_file() reads only the header, the sector allocation tables, the
// directory and the mini stream. Large streams, like EncryptedPackage, are
// read only on request.

struct OleCloser {
  void operator()(xls::OLE2* ole) const { xls::ole2_close(ole); }
};
struct OleStreamCloser {
  void operator()(xls::OLE2Stream* st) const { xls::ole2_fclose(st); }
};
typedef std::unique_ptr<xls::OLE2, OleCloser> OlePtr;
typedef std::unique_ptr<xls::OLE2Stream, OleStreamCloser> OleStreamPtr;

// libxls fopen()s the path, so it must be in the native encoding. Do this
// before acquiring any libxls resources, since it can raise an R error.
std::string native_path(const cpp11::strings& path) {
  return cpp11::safe[Rf_translateChar](static_cast<SEXP>(path[0]));
}

// Returns NULL if `path` is not a compound file libxls can read
OlePtr ole_open(const std::string& path) {
  return OlePtr(xls::ole2_open_file(path.c_str()));
}

bool ole_has_stream(const xls::OLE2* ole, const char* name) {
  for (long i = 0; i < ole->files.count; i++) {
    const char* x = ole->files.file[i].name;
    if (x != NULL && std::strcmp(x, name) == 0) {
      return true;
    }
  }
  return false;
}

std::string ole_read_stream(xls::OLE2* ole, const char* name) {
  OleStreamPtr st(xls::ole2_fopen(ole, name));
  if (!st) {
    throw std::runtime_error(
      std::string("Corrupt compound file: can't open stream ") + name + "."
    );
  }
  // Read in chunks rather than allocating the claimed size up front, so a
  // corrupt size can't trigger a huge allocation
  size_t size = st->size;
  std::string out;
  std::vector<char> buf(65536);
  while (out.size() < size) {
    size_t want = std::min(buf.size(), size - out.size());
    auto got = xls::ole2_read(buf.data(), 1, want, st.get());
    if (got <= 0) {
      throw std::runtime_error("Corrupt compound file: truncated stream.");
    }
    out.append(buf.data(), static_cast<size_t>(got));
  }
  return out;
}

// Cryptography -----------------------------------------------------------------

std::string aes_decrypt(const std::string& key, std::string data,
                        const std::string& iv = "") {
  if (data.size() % 16 != 0) {
    throw std::runtime_error("Encrypted data is not a multiple of 16 bytes.");
  }
  const uint8_t* k = reinterpret_cast<const uint8_t*>(key.data());
  const uint8_t* v = iv.empty() ? NULL
                                : reinterpret_cast<const uint8_t*>(iv.data());
  uint8_t* buf = reinterpret_cast<uint8_t*>(&data[0]);
  switch (key.size()) {
  case 16: readxl_aes128_decrypt(k, v, buf, data.size()); break;
  case 24: readxl_aes192_decrypt(k, v, buf, data.size()); break;
  case 32: readxl_aes256_decrypt(k, v, buf, data.size()); break;
  default: throw std::runtime_error("Unsupported AES key size.");
  }
  return data;
}

std::string hash(const std::string& alg, const std::string& data) {
  if (alg == "SHA1") return sha::sha1(data);
  if (alg == "SHA256") return sha::sha256(data);
  if (alg == "SHA384") return sha::sha512(data, 48);
  if (alg == "SHA512") return sha::sha512(data);
  throw std::runtime_error("Unsupported hash algorithm.");
}

// H(salt + password), then `spin_count` rounds of H(iterator + H)
std::string hash_password(const std::string& alg, const std::string& salt,
                          const std::string& password, uint32_t spin_count) {
  std::string h = hash(alg, salt + password);
  for (uint32_t i = 0; i < spin_count; i++) {
    h = hash(alg, le32_bytes(i) + h);
  }
  return h;
}

// Truncate, or pad with 0x36, to `len` bytes
std::string fit(std::string x, size_t len) {
  x.resize(len, '\x36');
  return x;
}

std::string base64_decode(const char* in) {
  static const std::string chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  std::string out;
  uint32_t buf = 0;
  int bits = 0;
  for (; *in != '\0' && *in != '='; in++) {
    size_t v = chars.find(*in);
    if (v == std::string::npos) {
      continue;
    }
    buf = (buf << 6) | static_cast<uint32_t>(v);
    bits += 6;
    if (bits >= 8) {
      bits -= 8;
      out += static_cast<char>((buf >> bits) & 0xff);
    }
  }
  return out;
}

// [MS-OFFCRYPTO] 2.3.4.5 - 2.3.4.9
bool decrypt_standard(const std::string& info, const std::string& package,
                      const std::string& password, std::string* out) {
  size_t header_size = le32(info, 8);
  size_t key_bytes = le32(info, 12 + 16) / 8;
  size_t verifier = 12 + header_size;
  if (le32(info, verifier) != 16 || verifier + 4 + 16 + 16 + 4 + 32 > info.size()) {
    throw std::runtime_error("Unsupported standard encryption header.");
  }
  std::string salt = info.substr(verifier + 4, 16);
  std::string enc_verifier = info.substr(verifier + 20, 16);
  std::string enc_verifier_hash = info.substr(verifier + 40, 32);

  std::string h = hash_password("SHA1", salt, password, 50000);
  h = sha::sha1(h + le32_bytes(0));
  std::string x1(64, '\x36'), x2(64, '\x5c');
  for (size_t i = 0; i < h.size(); i++) {
    x1[i] ^= h[i];
    x2[i] ^= h[i];
  }
  std::string key = (sha::sha1(x1) + sha::sha1(x2)).substr(0, key_bytes);

  std::string verifier_hash = sha::sha1(aes_decrypt(key, enc_verifier));
  if (aes_decrypt(key, enc_verifier_hash).substr(0, 20) != verifier_hash) {
    return false;
  }
  *out = aes_decrypt(key, package.substr(8, (package.size() - 8) / 16 * 16));
  return true;
}

// [MS-OFFCRYPTO] 2.3.4.10 - 2.3.4.13
bool decrypt_agile(const std::string& info, const std::string& package,
                   const std::string& password, std::string* out) {
  std::string xml = info.substr(8);
  rapidxml::xml_document<> doc;
  doc.parse<rapidxml::parse_strip_xml_namespaces>(&xml[0]);
  rapidxml::xml_node<>* root = doc.first_node("encryption");
  rapidxml::xml_node<>* key_data = root ? root->first_node("keyData") : NULL;
  rapidxml::xml_node<>* encryptors =
    root ? root->first_node("keyEncryptors") : NULL;
  // There may be several key encryptors, we want the password one.
  rapidxml::xml_node<>* enc_key = NULL;
  for (rapidxml::xml_node<>* node = encryptors ? encryptors->first_node() : NULL;
       node != NULL && enc_key == NULL; node = node->next_sibling()) {
    rapidxml::xml_node<>* candidate = node->first_node("encryptedKey");
    if (candidate && candidate->first_attribute("spinCount")) {
      enc_key = candidate;
    }
  }
  if (key_data == NULL || enc_key == NULL) {
    throw std::runtime_error("Unsupported agile encryption info.");
  }
  struct {
    rapidxml::xml_node<>* node;
    std::string operator()(const char* name) const {
      rapidxml::xml_attribute<>* attr = node->first_attribute(name);
      if (attr == NULL) {
        throw std::runtime_error(std::string("Missing attribute: ") + name);
      }
      return attr->value();
    }
  } kd = {key_data}, ek = {enc_key};
  if (kd("cipherAlgorithm") != "AES" || ek("cipherAlgorithm") != "AES") {
    throw std::runtime_error("Unsupported cipher.");
  }

  // Derive the key that encrypts the verifier and the package key
  std::string alg = ek("hashAlgorithm");
  std::string salt = base64_decode(ek("saltValue").c_str());
  size_t key_bytes = std::stoul(ek("keyBits")) / 8;
  // Excel uses 100,000; cap it so a crafted file can't hang R
  unsigned long spin_count = std::stoul(ek("spinCount"));
  if (spin_count > 10000000) throw std::runtime_error("Unsupported spin count.");
  std::string h = hash_password(alg, salt, password, spin_count);
  auto decrypt_attr = [&](const char* block_key, const char* attr) {
    std::string key = fit(hash(alg, h + std::string(block_key, 8)), key_bytes);
    return aes_decrypt(key, base64_decode(ek(attr).c_str()), fit(salt, 16));
  };
  std::string hash_input = decrypt_attr(
    "\xfe\xa7\xd2\x76\x3b\x4b\x9e\x79", "encryptedVerifierHashInput"
  );
  std::string hash_value = decrypt_attr(
    "\xd7\xaa\x0f\x6d\x30\x61\x34\x4e", "encryptedVerifierHashValue"
  );
  std::string expected = hash(alg, hash_input.substr(0, salt.size()));
  if (hash_value.substr(0, expected.size()) != expected) {
    return false;
  }
  std::string secret_key = decrypt_attr(
    "\x14\x6e\x0b\xe7\xab\xac\xd0\xd6", "encryptedKeyValue"
  ).substr(0, std::stoul(kd("keyBits")) / 8);

  // The package is encrypted in 4096-byte segments, each with its own IV
  std::string pkg_alg = kd("hashAlgorithm");
  std::string pkg_salt = base64_decode(kd("saltValue").c_str());
  out->clear();
  for (uint32_t i = 0; 8 + size_t(i) * 4096 < package.size(); i++) {
    std::string segment = package.substr(8 + size_t(i) * 4096, 4096);
    segment.resize(segment.size() / 16 * 16);
    std::string iv = fit(hash(pkg_alg, pkg_salt + le32_bytes(i)), 16);
    *out += aes_decrypt(secret_key, segment, iv);
  }
  return true;
}

} // namespace

// Classify a file by the streams in its OLE2 compound document:
// * "encrypted_xlsx": ECMA-376 encrypted Office document.
// * "xls": has a workbook stream, as libxls looks for in xls_open_file().
// * "other": some other compound document, e.g. a .doc.
// * NA: not a compound document libxls can read, e.g. a regular xlsx (zip).
//   libxls checks the signature first, so this is cheap.
[[cpp11::register]]
cpp11::r_string ole_kind_(cpp11::strings path) {
  OlePtr ole = ole_open(native_path(path));
  if (!ole) {
    return cpp11::na<cpp11::r_string>();
  }
  if (ole_has_stream(ole.get(), "EncryptionInfo") &&
      ole_has_stream(ole.get(), "EncryptedPackage")) {
    return "encrypted_xlsx";
  }
  if (ole_has_stream(ole.get(), "Workbook") ||
      ole_has_stream(ole.get(), "Book")) {
    return "xls";
  }
  return "other";
}

// Decrypt an encrypted xlsx. `password` is UTF-16LE. Returns the decrypted
// xlsx (zip) bytes, or NULL if the password is wrong.
[[cpp11::register]]
cpp11::sexp xlsx_decrypt_(cpp11::strings path, cpp11::raws password) {
  std::string native = native_path(path);
  std::string pass = to_string(password);
  std::string info, package;
  {
    OlePtr ole = ole_open(native);
    if (!ole) {
      throw std::runtime_error("Can't read compound file.");
    }
    info = ole_read_stream(ole.get(), "EncryptionInfo");
    package = ole_read_stream(ole.get(), "EncryptedPackage");
  }
  if (info.size() < 8 || package.size() < 8) {
    throw std::runtime_error("Corrupt encrypted file.");
  }

  uint32_t version = le32(info, 0);
  std::string out;
  bool ok;
  if (version == 0x00040004) {
    ok = decrypt_agile(info, package, pass, &out);
  } else if ((version >> 16) == 2) {
    ok = decrypt_standard(info, package, pass, &out);
  } else {
    throw std::runtime_error("Unsupported encryption version.");
  }
  if (!ok) {
    return R_NilValue;
  }

  // The package starts with its unencrypted size
  size_t size = le32(package, 0);
  if (size > out.size()) {
    throw std::runtime_error("Corrupt encrypted file: package too short.");
  }
  cpp11::writable::raws result(static_cast<R_xlen_t>(size));
  std::memcpy(RAW(result), out.data(), size);
  return result;
}
