#ifndef EXELL_XLSWORKBOOK_
#define EXELL_XLSWORKBOOK_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "CellType.h"

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

public:

  XlsWorkBook(std::string path) {
    path_ = normalizePath(path);
    pWB_ = xls::xls_open(path_.c_str(), "UTF8");
  }

  ~XlsWorkBook() {
    try {
      xls_close_WB(pWB_);
    } catch(...) {}
  }

  xls::xlsWorkBook* workbook() const {
    return pWB_;
  }

  int nSheets() const {
    return pWB_->sheets.count;
  }

  std::vector<std::string> sheets() const {
    std::vector<std::string> sheets;
    sheets.reserve(nSheets());

    for (int i = 0; i < nSheets(); ++i) {
      std::string name((char*) pWB_->sheets.sheet[i].name);
      sheets.push_back(name);
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


  XlsWorkSheet sheet(std::string name);
  XlsWorkSheet sheet(int i);

};

#endif
