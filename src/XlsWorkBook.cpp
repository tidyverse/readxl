#include <Rcpp.h>

# pragma GCC diagnostic ignored "-Wc99-extensions"
# pragma clang diagnostic ignored "-Wc99-extensions"

#include "XlsWorkBook.h"
using namespace Rcpp;

// [[Rcpp::export]]
CharacterVector xls_sheets(std::string path) {
  XlsWorkBook wb(path);
  return wb.sheets();
}

// [[Rcpp::export]]
std::set<int> xls_date_formats(std::string path) {
  return XlsWorkBook(path).dateFormats();
}
