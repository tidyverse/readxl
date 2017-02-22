#include <Rcpp.h>

#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
#include <libxls/xls.h>

using namespace Rcpp;

// [[Rcpp::export]]
CharacterVector xls_col_names(std::string path, int i = 0, int nskip = 0) {
  return XlsWorkSheet(path, i, nskip).colNames();
}

// [[Rcpp::export]]
CharacterVector xls_col_types(std::string path,
                              std::vector<std::string> na,
                              int sheet = 0, int nskip = 0,
                              int guess_max = 1000, bool has_col_names = false) {
  XlsWorkSheet ws(path, sheet, nskip);
  std::vector<ColType> types = ws.colTypes(na, guess_max, has_col_names);

  CharacterVector out(types.size());
  for (size_t i = 0; i < types.size(); ++i) {
    out[i] = colTypeDesc(types[i]);
  }

  return out;
}

// [[Rcpp::export]]
List xls_cols(std::string path, int i, CharacterVector col_names,
              CharacterVector col_types, std::vector<std::string> na,
              int nskip = 0) {
  XlsWorkSheet ws(path, i, nskip);
  std::vector<ColType> types = colTypeStrings(col_types);
  return ws.readCols(col_names, types, na, nskip);
}
