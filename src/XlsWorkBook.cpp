#define R_NO_REMAP
#define STRICT_R_HEADERS

#include "XlsWorkBook.h"
#include <Rcpp.h>
using namespace Rcpp;

[[cpp11::register]]
CharacterVector xls_sheets(std::string path) {
  XlsWorkBook wb(path);
  return wb.sheets();
}

[[cpp11::register]]
std::set<int> xls_date_formats(std::string path) {
  return XlsWorkBook(path).dateFormats();
}
