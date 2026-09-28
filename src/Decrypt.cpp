#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>

#include "cpp11/R.hpp"
#include "cpp11/r_string.hpp"

// The vendored msoffice headers use printf/fprintf for optional debug output.
// Include cran.h to mask those to Rprintf/Rprintf2 for R CMD check, the same
// approach libxls uses via src/libxls/xlstypes.h. cran.h must come after the
// system/stdio includes above so its `#define printf Rprintf` only affects the
// msoffice headers included below.
#include "cran.h"

#include "msoffice/decode.hpp"
#include "msoffice/util.hpp"

namespace {

std::string path_to_native(const std::string& path) {
  return std::string(Rf_translateChar(cpp11::r_string(path)));
}

std::string read_file_bytes(const std::string& path) {
  const std::string native_path = path_to_native(path);
  std::ifstream ifs(native_path.c_str(), std::ios::binary);
  if (!ifs) {
    throw std::runtime_error("Failed to open file for reading.");
  }
  return std::string(
    (std::istreambuf_iterator<char>(ifs)),
    std::istreambuf_iterator<char>()
  );
}

bool has_stream(const ms::cfb::CompoundFile& cfb, const char* name) {
  const cybozu::String16 wname = cybozu::ToUtf16(name);
  for (size_t i = 0; i < cfb.dirs.size(); i++) {
    if (cfb.dirs[i].directoryEntryName == wname) {
      return true;
    }
  }
  return false;
}

// UTF-8 -> UTF-16LE bytes, as expected by [MS-OFFCRYPTO]
std::string password_to_utf16(const std::string& password) {
  if (password.empty()) {
    return std::string();
  }
  return ms::Char16toChar8(cybozu::ToUtf16(password));
}

} // namespace

// Probe an OLE2 (compound document) file for the streams that mark it as an
// ECMA-376 encrypted Office document, as opposed to a legacy .xls file.
[[cpp11::register]]
bool is_encrypted_xlsx_(std::string path) {
  try {
    const std::string native_path = path_to_native(path);
    // Cheap gate: only OLE2 (compound document) files can be encrypted Office
    // documents, so peek at the 8-byte signature before reading the whole file.
    std::ifstream ifs(native_path.c_str(), std::ios::binary);
    if (!ifs) {
      return false;
    }
    char sig[8] = {0};
    ifs.read(sig, sizeof(sig));
    if (ifs.gcount() < 8 || ms::DetectFormat(sig, sizeof(sig)) != ms::fCfb) {
      return false;
    }
    ifs.close();

    const std::string data = read_file_bytes(path);
    ms::cfb::CompoundFile cfb(
      data.c_str(),
      static_cast<uint32_t>(data.size())
    );
    return has_stream(cfb, "EncryptionInfo") &&
      has_stream(cfb, "EncryptedPackage");
  } catch (...) {
    return false;
  }
}

// Decrypt an ECMA-376 encrypted Office document to `out_path`.
// Returns TRUE on success, FALSE if the password is wrong.
[[cpp11::register]]
bool xlsx_decrypt_(std::string path,
                   std::string password,
                   std::string out_path) {
  const std::string data = read_file_bytes(path);
  const std::string pass = password_to_utf16(password);
  const std::string native_out_path = path_to_native(out_path);
  std::string secretKey;
  bool ok = ms::decode(
    data.data(),
    static_cast<uint32_t>(data.size()),
    native_out_path,
    pass,
    secretKey,
    /* doView = */ false
  );
  return ok;
}
