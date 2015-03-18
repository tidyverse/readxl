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

// [[Rcpp::export]]
CharacterVector xlsx_col_types(std::string path, std::string sheet = "sheet1",
                               std::string na = "", int nskip = 0, int n = 100) {

  XlsxWorkSheet ws(path, sheet);
  std::vector<CellType> types = ws.colTypes(na, nskip, n);

  CharacterVector out(types.size());
  for (int i = 0; i < types.size(); ++i) {
    out[i] = cellTypeDesc(types[i]);
  }

  return out;
}
