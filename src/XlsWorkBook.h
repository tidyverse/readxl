#ifndef READXL_XLSWORKBOOK_
#define READXL_XLSWORKBOOK_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "ColSpec.h"
#include "utils.h"

inline std::string normalizePath(std::string path) {
  Rcpp::Environment baseEnv = Rcpp::Environment::base_env();
  Rcpp::Function normalizePath = baseEnv["normalizePath"];
  return Rcpp::as<std::string>(normalizePath(path, "/", true));
}

class XlsWorkBook {

  // common to Xls[x]WorkBook
  std::string path_;
  bool is1904_;
  std::set<int> dateStyles_;

  // kept as data + accessor in XlsWorkBook vs. member function in XlsxWorkBook
  int n_sheets_;
  Rcpp::CharacterVector sheets_;

public:

  XlsWorkBook(const std::string& path) {
    path_ = normalizePath(path);

    xls::xlsWorkBook* pWB_ = xls::xls_open(path_.c_str(), "UTF-8");
    if (pWB_ == NULL) {
      Rcpp::stop("Failed to open %s", path);
    }

    n_sheets_ = pWB_->sheets.count;
    sheets_ = Rcpp::CharacterVector(n_sheets());
    for (int i = 0; i < n_sheets_; ++i) {
      sheets_[i] = Rf_mkCharCE((char*) pWB_->sheets.sheet[i].name, CE_UTF8);
    }

    is1904_ = pWB_->is1904;

    int n_formats = pWB_->formats.count;
    for (int i = 0; i < n_formats; ++i) {
      xls::st_format::st_format_data format = pWB_->formats.format[i];
      std::string value((char*) format.value);
      if (isDateFormat(value)) {
        dateStyles_.insert(format.index);
      }
    }

    xls::xls_close_WB(pWB_);
  }

  const std::string& path() const{
    return path_;
  }

  int n_sheets() const {
    return n_sheets_;
  }

  Rcpp::CharacterVector sheets() const {
    return sheets_;
  }

  bool is1904() const {
    return is1904_;
  }

  const std::set<int>& dateStyles() const {
    return dateStyles_;
  }

};

#endif
