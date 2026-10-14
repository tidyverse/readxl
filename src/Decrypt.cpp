// Decryption of password-protected xlsx files, as specified in [MS-OFFCRYPTO].
//
// An encrypted xlsx is an OLE2 compound file (CFB) holding two streams:
// `EncryptionInfo`, which describes how the key is derived from the password,
// and `EncryptedPackage`, which is the AES-encrypted xlsx zip. We support both
// variants of ECMA-376 encryption:
//
// * Standard (Office 2007): SHA-1 key derivation, AES-ECB.
// * Agile (Office 2010 and later): SHA-1/SHA-2 key derivation, AES-CBC.

#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include "cpp11/raws.hpp"
#include "cpp11/sexp.hpp"

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

// Compound File Binary format ([MS-CFB]) --------------------------------------

const uint32_t END_OF_CHAIN = 0xFFFFFFFE;
const char CFB_SIGNATURE[] = "\xD0\xCF\x11\xE0\xA1\xB1\x1A\xE1";

class CompoundFile {
  const std::string& data_;
  size_t sector_size_;
  uint32_t mini_cutoff_;
  std::vector<uint32_t> fat_, minifat_;
  std::string dir_, ministream_;

  std::string sector(uint32_t id) const {
    size_t pos = (static_cast<size_t>(id) + 1) * sector_size_;
    if (pos + sector_size_ > data_.size()) {
      throw std::runtime_error("Corrupt compound file: sector out of range.");
    }
    return data_.substr(pos, sector_size_);
  }

  // Concatenate the sectors in the chain starting at `start`. Sectors are
  // `size` bytes, read from `pool` (the mini stream) or from the file itself.
  std::string chain(uint32_t start, const std::vector<uint32_t>& fat,
                    size_t size, const std::string* pool = NULL) const {
    std::string out;
    for (uint32_t id = start; id != END_OF_CHAIN; id = fat[id]) {
      if (id >= fat.size() || out.size() > data_.size()) {
        throw std::runtime_error("Corrupt compound file: bad sector chain.");
      }
      if (pool == NULL) {
        out += sector(id);
      } else if ((id + 1) * size <= pool->size()) {
        out += pool->substr(id * size, size);
      } else {
        throw std::runtime_error("Corrupt compound file: bad mini sector.");
      }
    }
    return out;
  }

  static std::vector<uint32_t> as_u32(const std::string& s) {
    std::vector<uint32_t> out(s.size() / 4);
    for (size_t i = 0; i < out.size(); i++) {
      out[i] = le32(s, 4 * i);
    }
    return out;
  }

public:
  explicit CompoundFile(const std::string& data) : data_(data) {
    if (data.size() < 512 || data.compare(0, 8, CFB_SIGNATURE, 8) != 0) {
      throw std::runtime_error("Not a compound file.");
    }
    sector_size_ = size_t(1) << (le32(data, 0x1C) >> 16);
    if (sector_size_ != 512 && sector_size_ != 4096) {
      throw std::runtime_error("Corrupt compound file: bad sector size.");
    }
    mini_cutoff_ = le32(data, 0x38);

    // The FAT is stored in sectors listed in the DIFAT: the first 109 entries
    // are in the header, the rest in a chain of DIFAT sectors.
    std::vector<uint32_t> fat_sectors;
    for (int i = 0; i < 109; i++) {
      fat_sectors.push_back(le32(data, 0x4C + 4 * i));
    }
    uint32_t difat = le32(data, 0x44);
    for (uint32_t n = le32(data, 0x48); n > 0 && difat < 0xFFFFFFFA; n--) {
      std::vector<uint32_t> entries = as_u32(sector(difat));
      difat = entries.back();
      fat_sectors.insert(fat_sectors.end(), entries.begin(), entries.end() - 1);
    }
    std::string fat;
    for (size_t i = 0; i < fat_sectors.size() && fat_sectors[i] < 0xFFFFFFFA;
         i++) {
      fat += sector(fat_sectors[i]);
    }
    fat_ = as_u32(fat);

    dir_ = chain(le32(data, 0x30), fat_, sector_size_);
    minifat_ = as_u32(chain(le32(data, 0x3C), fat_, sector_size_));
    // The root directory entry holds the start and size of the mini stream
    ministream_ = chain(le32(dir_, 0x74), fat_, sector_size_)
                    .substr(0, le32(dir_, 0x78));
  }

  // Returns false if there is no stream named `name` (ASCII only).
  bool stream(const std::string& name, std::string* out = NULL) const {
    for (size_t pos = 0; pos + 128 <= dir_.size(); pos += 128) {
      size_t name_bytes = le32(dir_, pos + 0x40) & 0xFFFF;
      int type = dir_[pos + 0x42];
      if (type != 2 || name_bytes != 2 * (name.size() + 1)) {
        continue;
      }
      bool match = true;
      for (size_t i = 0; i < name.size() && match; i++) {
        match = dir_[pos + 2 * i] == name[i] && dir_[pos + 2 * i + 1] == 0;
      }
      if (!match) {
        continue;
      }
      if (out != NULL) {
        uint32_t start = le32(dir_, pos + 0x74);
        uint32_t size = le32(dir_, pos + 0x78);
        *out = size < mini_cutoff_ ? chain(start, minifat_, 64, &ministream_)
                                   : chain(start, fat_, sector_size_);
        if (out->size() < size) {
          throw std::runtime_error("Corrupt compound file: truncated stream.");
        }
        out->resize(size);
      }
      return true;
    }
    return false;
  }
};

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
  throw std::runtime_error("Unsupported hash algorithm: " + alg);
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
    throw std::runtime_error("Unsupported cipher: " + ek("cipherAlgorithm"));
  }

  // Derive the key that encrypts the verifier and the package key
  std::string alg = ek("hashAlgorithm");
  std::string salt = base64_decode(ek("saltValue").c_str());
  size_t key_bytes = std::stoul(ek("keyBits")) / 8;
  size_t block_size = std::stoul(ek("blockSize"));
  std::string h = hash_password(alg, salt, password, std::stoul(ek("spinCount")));
  auto decrypt_attr = [&](const char* block_key, const char* attr) {
    std::string key = fit(hash(alg, h + std::string(block_key, 8)), key_bytes);
    return aes_decrypt(key, base64_decode(ek(attr).c_str()), fit(salt, block_size));
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
  size_t pkg_block_size = std::stoul(kd("blockSize"));
  out->clear();
  for (uint32_t i = 0; 8 + size_t(i) * 4096 < package.size(); i++) {
    std::string segment = package.substr(8 + size_t(i) * 4096, 4096);
    segment.resize(segment.size() / 16 * 16);
    std::string iv = fit(hash(pkg_alg, pkg_salt + le32_bytes(i)), pkg_block_size);
    *out += aes_decrypt(secret_key, segment, iv);
  }
  return true;
}

} // namespace

// Is this an OLE2 (compound document) file holding an ECMA-376 encrypted
// Office document, as opposed to e.g. a legacy .xls file?
[[cpp11::register]]
bool is_encrypted_xlsx_(cpp11::raws data) {
  try {
    std::string bytes = to_string(data);
    CompoundFile cfb(bytes);
    return cfb.stream("EncryptionInfo") && cfb.stream("EncryptedPackage");
  } catch (...) {
    return false;
  }
}

// Decrypt an encrypted xlsx. `password` is UTF-16LE. Returns the decrypted
// xlsx (zip) bytes, or NULL if the password is wrong.
[[cpp11::register]]
cpp11::sexp xlsx_decrypt_(cpp11::raws data, cpp11::raws password) {
  std::string bytes = to_string(data);
  std::string pass = to_string(password);
  CompoundFile cfb(bytes);
  std::string info, package;
  cfb.stream("EncryptionInfo", &info);
  cfb.stream("EncryptedPackage", &package);
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
