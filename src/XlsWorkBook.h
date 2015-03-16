#include <Rcpp.h>
#include <libxls/xls.h>

inline std::string normalizePath(std::string path) {
  Rcpp::Environment baseEnv = Rcpp::Environment::base_env();
  Rcpp::Function normalizePath = baseEnv["normalizePath"];
  return Rcpp::as<std::string>(normalizePath(path, false));
}

class XlsWorkBook {
  std::string path_;
  xls::xlsWorkBook* pWB_;

public:

  XlsWorkBook(std::string path) {
    path_ = normalizePath(path);
    pWB_ = xls::xls_open(path_.c_str(), "UTF8");

    if (pWB_ == NULL)
      Rcpp::stop("Failed to open %s", path);
  }

  ~XlsWorkBook() {
    try {
      xls_close(pWB_);
    } catch(...) {}
  }

  int nSheets() {
    return pWB_->sheets.count;
  }

  std::vector<std::string> sheets() {
    std::vector<std::string> sheets;
    sheets.reserve(nSheets());

    for (int i = 0; i < nSheets(); ++i) {
      std::string name((char*) pWB_->sheets.sheet[i].name);
      sheets.push_back(name);
    }

    return sheets;
  }

};
