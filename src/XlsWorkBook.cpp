#include <Rcpp.h>
#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
using namespace Rcpp;

XlsWorkSheet XlsWorkBook::sheet(int sheet_i, int nskip) {
  return XlsWorkSheet(*this, sheet_i, nskip);
}

// [[Rcpp::export]]
std::map<int,std::string> xls_formats(std::string path) {
  XlsWorkBook wb(path);
  return wb.formats();
}

// [[Rcpp::export]]
CharacterVector xls_sheets(std::string path) {
  XlsWorkBook wb(path);
  return wb.sheets();
}
