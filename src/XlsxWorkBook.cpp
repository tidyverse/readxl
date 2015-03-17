#include <Rcpp.h>
using namespace Rcpp;
#include "XlsxWorkBook.h"

// [[Rcpp::export]]
std::vector<std::string> xlsx_sheets(std::string path) {
  return XlsxWorkBook(path).sheets();
}

// [[Rcpp::export]]
std::vector<std::string> xlsx_strings(std::string path) {
  return XlsxWorkBook(path).strings();
}
