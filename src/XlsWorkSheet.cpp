#include <Rcpp.h>

#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
#include <libxls/xls.h>

using namespace Rcpp;

// [[Rcpp::export]]
CharacterVector xls_col_names(std::string path, int i = 0, int nskip = 0) {
  XlsWorkBook wb = XlsWorkBook(path);
  return wb.sheet(i).colNames(nskip);
}

// [[Rcpp::export]]
CharacterVector xls_col_types(std::string path, std::vector<std::string> na, int sheet = 0,
                              int nskip = 0, int n = 100, bool has_col_names = false) {
  XlsWorkBook wb = XlsWorkBook(path);
  std::vector<CellType> types = wb.sheet(sheet).colTypes(na, nskip + has_col_names, n);

  CharacterVector out(types.size());
  for (size_t i = 0; i < types.size(); ++i) {
    out[i] = cellTypeDesc(types[i]);
  }

  if (has_col_names) {
    // blank columns with a name aren't blank
    CharacterVector names = xls_col_names(path, sheet, nskip);
    for (size_t i = 0; i < types.size(); ++i) {
      if (types[i] == CELL_BLANK && names[i] != NA_STRING && names[i] != "")
        out[i] = cellTypeDesc(CELL_NUMERIC);
    }
  }

  return out;
}

// [[Rcpp::export]]
List xls_cols(std::string path, int i, CharacterVector col_names,
              CharacterVector col_types, std::vector<std::string> na, int nskip = 0) {
  XlsWorkBook wb = XlsWorkBook(path);
  XlsWorkSheet sheet = wb.sheet(i);

  if (col_names.size() != col_types.size())
    stop("`col_names` and `col_types` must have the same length");

  std::vector<CellType> types = cellTypes(col_types);
  return sheet.readCols(col_names, types, na, nskip);
}
