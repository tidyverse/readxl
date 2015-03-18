#include <Rcpp.h>
#include "XlsxWorkBook.h"
using namespace Rcpp;

// [[Rcpp::export]]
std::vector<std::string> xlsx_sheets(std::string path) {
  return XlsxWorkBook(path).sheets();
}

// [[Rcpp::export]]
std::vector<std::string> xlsx_strings(std::string path) {
  return XlsxWorkBook(path).strings();
}

// [[Rcpp::export]]
std::set<int> xlsx_date_styles(std::string path) {
  return XlsxWorkBook(path).dateStyles();
}

// [[Rcpp::export]]
IntegerVector parse_ref(std::string ref) {
  std::pair<int,int> parsed = parseRef(ref);

  return IntegerVector::create(parsed.first, parsed.second);
}
