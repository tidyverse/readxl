#include <Rcpp.h>

#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
#include <libxls/xls.h>

using namespace Rcpp;

// [[Rcpp::export]]
CharacterVector xls_col_types(std::string path, std::string na, int i = 0,
                              int nskip = 0, int n = 100) {
  std::vector<CellType> types = XlsWorkBook(path).sheet(i).colTypes(na, nskip, n);

  CharacterVector out(types.size());
  for (int i = 0; i < types.size(); ++i) {
    switch(types[i]) {
    case CELL_BLANK:   out[i] = "blank"  ; break;
    case CELL_DATE:    out[i] = "date"   ; break;
    case CELL_NUMERIC: out[i] = "numeric"; break;
    case CELL_TEXT:    out[i] = "text"   ; break;
    }
  }

  return out;
}

// [[Rcpp::export]]
CharacterVector xls_col_names(std::string path, int i = 0, int nskip = 0) {
  return XlsWorkBook(path).sheet(i).colNames(nskip);
}

// [[Rcpp::export]]
List xls_cols(std::string path, int i, CharacterVector col_names,
              CharacterVector col_types, std::string na, int nskip = 0) {
  XlsWorkSheet sheet = XlsWorkBook(path).sheet(i);

  if (col_names.size() != col_types.size())
    stop("`col_names` and `col_types` must have the same length");

  std::vector<CellType> types = cellTypes(col_types);
  return sheet.readCols(col_names, types, na, nskip);
}
