#include <Rcpp.h>
#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
using namespace Rcpp;

XlsWorkSheet XlsWorkBook::sheet(int i) {
  return XlsWorkSheet(*this, i);
}
XlsWorkSheet XlsWorkBook::sheet(std::string name) {
  for (int i = 0; i < nSheets(); ++i) {
    std::string this_name((char*) pWB_->sheets.sheet[i].name);
    if (this_name == name)
      return sheet(i);
  }

  stop("Couldn't find sheet called '%s'", name);
  return sheet(0);
}

// [[Rcpp::export]]
std::map<int,std::string> xls_formats(std::string path) {
  XlsWorkBook wb(path);
  return wb.formats();
}

// [[Rcpp::export]]
std::vector<std::string> xls_sheets(std::string path) {
  XlsWorkBook wb(path);
  return wb.sheets();
}
