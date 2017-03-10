#include <Rcpp.h>
#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
using namespace Rcpp;

// [[Rcpp::export]]
CharacterVector xls_sheets(std::string path) {
  XlsWorkBook wb(path);
  return wb.sheets();
}
