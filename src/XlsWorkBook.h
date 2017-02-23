#ifndef READXL_XLSWORKBOOK_
#define READXL_XLSWORKBOOK_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "CellType.h"
#include "utils.h"

inline std::string normalizePath(std::string path) {
  Rcpp::Environment baseEnv = Rcpp::Environment::base_env();
  Rcpp::Function normalizePath = baseEnv["normalizePath"];
  return Rcpp::as<std::string>(normalizePath(path, "/", true));
}

class XlsWorkSheet;

typedef std::map<int,std::string> FormatMap;

class XlsWorkBook {
  std::string path_;
  xls::xlsWorkBook* pWB_;
  std::set<int> customDateFormats_;
  double offset_;

public:

  XlsWorkBook(const std::string& path)
  {
    path_ = normalizePath(path);
    pWB_ = xls::xls_open(path_.c_str(), "UTF-8");
    if (pWB_ == NULL) {
      Rcpp::stop("Failed to open %s", path);
    }
    offset_ = dateOffset(pWB_->is1904);
    customDateFormats_ = customDateFormats();

  }

  ~XlsWorkBook() {
    try {
      xls_close_WB(pWB_);
    } catch(...) {}
  }

  xls::xlsWorkBook* workbook() const {
    return pWB_;
  }

  int n_sheets() const {
    return pWB_->sheets.count;
  }

  Rcpp::CharacterVector sheets() const {
    Rcpp::CharacterVector sheets(n_sheets());

    for (int i = 0; i < n_sheets(); ++i) {
      sheets[i] = Rf_mkCharCE((char*) pWB_->sheets.sheet[i].name, CE_UTF8);
    }

    return sheets;
  }

  int nFormats() const {
    return pWB_->formats.count;
  }

  FormatMap formats() const {
    std::map<int, std::string> formats;

    for (int i = 0; i < nFormats(); ++i) {
      xls::st_format::st_format_data format = pWB_->formats.format[i];
      std::string value((char*) pWB_->formats.format[i].value);

      formats.insert(std::make_pair(format.index, value));
    }

    return formats;
  }

  XlsWorkSheet sheet(int sheet_i, int nskip);

  const std::set<int>& customDateFormats() {
    return customDateFormats_;
  }

  double offset() {
    return offset_;
  }

private:

  std::set<int> customDateFormats() const {
    std::set<int> dateFormats;

    for (int i = 0; i < nFormats(); ++i) {
      xls::st_format::st_format_data format = pWB_->formats.format[i];
      std::string value((char*) format.value);

      if (isDateFormat(value))
        dateFormats.insert(format.index);
    }

    return dateFormats;
  }

};

#endif
