#include <Rcpp.h>
#include "XlsxWorkSheet.h"
using namespace Rcpp;

// [[Rcpp::export]]
void xlsx_cells(std::string path, std::string sheet) {
  return XlsxWorkSheet(path, sheet).printCells();
}

// [[Rcpp::export]]
IntegerVector parse_ref(std::string ref) {
  std::pair<int,int> parsed = parseRef(ref);

  return IntegerVector::create(parsed.first, parsed.second);
}
