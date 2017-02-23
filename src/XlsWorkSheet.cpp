#include <Rcpp.h>

#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
#include "ColSpec.h"
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

  return out;
}

// [[Rcpp::export]]
List xls_cols(std::string path, int i, CharacterVector col_names,
              CharacterVector col_types, std::vector<std::string> na,
              int nskip = 0) {
  XlsWorkBook wb = XlsWorkBook(path);
  std::vector<ColType> types = colTypeStrings(col_types);
  XlsWorkSheet sheet = wb.sheet(i, nskip);
  return sheet.readCols(col_names, types, na, nskip);
}

// [[Rcpp::export]]
List read_xls_(std::string path, int sheet_i, RObject col_names,
                RObject col_types, std::vector<std::string> na,
                int nskip = 0, int guess_max = 1000) {
  XlsWorkBook wb = XlsWorkBook(path);
  XlsWorkSheet ws = wb.sheet(sheet_i, nskip);

  // catches empty sheets and sheets where we skip past all data
  if (ws.nrow() == 0 && ws.ncol() == 0) {
   return Rcpp::List(0);
  }

  // Get column names --------------------------------------------------
  CharacterVector colNames;
  bool sheetHasColumnNames = false;
  switch(TYPEOF(col_names)) {
  case STRSXP:
    colNames = as<CharacterVector>(col_names);
    break;
  case LGLSXP:
  {
    sheetHasColumnNames = as<bool>(col_names);
    colNames = sheetHasColumnNames ? ws.colNames() : CharacterVector(ws.ncol(), "");
    break;
  }
  default:
    Rcpp::stop("`col_names` must be a logical or character vector");
  }

  // Get column types --------------------------------------------------
  std::vector<ColType> colTypes;
  switch(TYPEOF(col_types)) {
  case NILSXP:
    colTypes = ws.colTypes(na, guess_max, sheetHasColumnNames);
    break;
  case STRSXP:
    colTypes = colTypeStrings(as<CharacterVector>(col_types));
    colTypes = recycleTypes(colTypes, ws.ncol());
    break;
  default:
    Rcpp::stop("`col_types` must be a character vector or NULL");
  }
  if ((int) colTypes.size() != ws.ncol()) {
    Rcpp::stop("Sheet %d has %d columns, but `col_types` has length %d.",
               sheet_i + 1, ws.ncol(), colTypes.size());
  }

  // convert blank columns to a default type (numeric today, but logical soon)
  // can only happen when
  //   * col_types = NULL and we've learned them from data
  //   * all cells in column are empty or match one of the na strings
  for (size_t i = 0; i < colTypes.size(); i++) {
    if (colTypes[i] == COL_BLANK) {
      colTypes[i] = COL_NUMERIC;
    }
  }

  colNames = reconcileNames(colNames, colTypes, sheet_i);

  return ws.readCols(colNames, colTypes, na, sheetHasColumnNames);
}
