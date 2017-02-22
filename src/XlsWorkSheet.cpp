#include <Rcpp.h>

#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
#include <libxls/xls.h>

using namespace Rcpp;

// [[Rcpp::export]]
CharacterVector xls_col_names(std::string path, int i = 0, int nskip = 0) {
  XlsWorkBook wb = XlsWorkBook(path);
  return wb.sheet(i, nskip).colNames();
}

// [[Rcpp::export]]
CharacterVector xls_col_types(std::string path,
                              std::vector<std::string> na,
                              int sheet = 0, int nskip = 0,
                              int guess_max = 1000, bool has_col_names = false) {
  XlsWorkBook wb = XlsWorkBook(path);
  std::vector<ColType> types = wb.sheet(sheet, nskip).colTypes(na,
                                        guess_max, has_col_names);

  CharacterVector out(types.size());
  for (size_t i = 0; i < types.size(); ++i) {
    out[i] = colTypeDesc(types[i]);
  }

  if (has_col_names) {
    // blank columns with a name aren't blank
    CharacterVector names = xls_col_names(path, sheet, nskip);
    for (size_t i = 0; i < types.size(); ++i) {
      if (types[i] == COL_BLANK && names[i] != NA_STRING && names[i] != "")
        out[i] = colTypeDesc(COL_NUMERIC);
    }
  }

  return out;
}

// [[Rcpp::export]]
List xls_cols(std::string path, int i, CharacterVector col_names,
              CharacterVector col_types, std::vector<std::string> na,
              int nskip = 0) {
  XlsWorkBook wb = XlsWorkBook(path);
  XlsWorkSheet sheet = wb.sheet(i, nskip);

  if (col_names.size() != col_types.size()) {
    Rcpp::stop("Received %d names but %d types.",
               col_names.size(), col_types.size());
  }

  std::vector<ColType> types = colTypeStrings(col_types);
  return sheet.readCols(col_names, types, na, nskip);
}
