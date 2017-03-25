#include <Rcpp.h>
#include "XlsxWorkBook.h"
using namespace Rcpp;

// [[Rcpp::export]]
CharacterVector xlsx_sheets(std::string path) {
  return XlsxWorkBook(path).sheets();
}

// [[Rcpp::export]]
std::vector<std::string> xlsx_strings(std::string path) {
  return XlsxWorkBook(path).stringTable();
}

// [[Rcpp::export]]
std::set<int> xlsx_date_formats(std::string path) {
  return XlsxWorkBook(path).dateFormats();
}
