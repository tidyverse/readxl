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
    pWB_ = xls::xls_open(path.c_str(), "UTF8");

    if (pWB_ == NULL)
      Rcpp::stop("Failed to open %s", path);
  }

  ~XlsWorkBook() {
    try {
      xls_close(pWB_);
    } catch(...) {}
  }
};
