#include <Rcpp.h>

#include "XlsWorkBook.h"
#include "XlsWorkSheet.h"
#include "ColSpec.h"
#include <libxls/xls.h>

using namespace Rcpp;

// [[Rcpp::export]]
CharacterVector xls_col_names(std::string path,
                              std::vector<std::string> na,
                              int sheet_i = 0, int skip = 0) {
  XlsWorkBook wb = XlsWorkBook(path);
  return wb.sheet(sheet_i, skip).colNames(na);
}

// [[Rcpp::export]]
CharacterVector xls_col_types(std::string path,
                              std::vector<std::string> na,
                              int sheet_i = 0, int skip = 0,
                              int guess_max = 1000, bool has_col_names = false) {
  XlsWorkBook wb = XlsWorkBook(path);
  std::vector<ColType> types(wb.sheet(sheet_i, skip).ncol());
  std::fill(types.begin(), types.end(), COL_UNKNOWN);
  types = wb.sheet(sheet_i, skip).colTypes(types, na, guess_max, has_col_names);

  return colTypeDescs(types);
}

// [[Rcpp::export]]
List xls_cols(std::string path, int sheet_i, CharacterVector col_names,
              CharacterVector col_types, std::vector<std::string> na,
              int skip = 0) {
  XlsWorkBook wb = XlsWorkBook(path);
  std::vector<ColType> types = colTypeStrings(col_types);
  XlsWorkSheet sheet = wb.sheet(sheet_i, skip);
  return sheet.readCols(col_names, types, na, skip);
}

// [[Rcpp::export]]
List read_xls_(std::string path, int sheet_i, RObject col_names,
                RObject col_types, std::vector<std::string> na,
                int skip = 0, int guess_max = 1000) {
  XlsWorkBook wb = XlsWorkBook(path);
  XlsWorkSheet ws = wb.sheet(sheet_i, skip);

  // catches empty sheets and sheets where we skip past all data
  if (ws.nrow() == 0 && ws.ncol() == 0) {
   return Rcpp::List(0);
  }

  // Get column names --------------------------------------------------
  CharacterVector colNames;
  bool has_col_names = false;
  switch(TYPEOF(col_names)) {
  case STRSXP:
    colNames = as<CharacterVector>(col_names);
    break;
  case LGLSXP:
  {
    has_col_names = as<bool>(col_names);
    colNames = has_col_names ? ws.colNames(na) : CharacterVector(ws.ncol(), "");
    break;
  }
  default:
    Rcpp::stop("`col_names` must be a logical or character vector");
  }

  // Get column types --------------------------------------------------
  if (TYPEOF(col_types) != STRSXP) {
    Rcpp::stop("`col_types` must be a character vector");
  }
  std::vector<ColType> colTypes = colTypeStrings(as<CharacterVector>(col_types));
  colTypes = recycleTypes(colTypes, ws.ncol());
  if ((int) colTypes.size() != ws.ncol()) {
    Rcpp::stop("Sheet %d has %d columns, but `col_types` has length %d.",
               sheet_i + 1, ws.ncol(), colTypes.size());
  }
  if (requiresGuess(colTypes)) {
    colTypes = ws.colTypes(colTypes, na, guess_max, has_col_names);
  }
  colTypes = finalizeTypes(colTypes);

  // Reconcile column names and types ----------------------------------
  colNames = reconcileNames(colNames, colTypes, sheet_i);

  // Get data ----------------------------------------------------------
  return ws.readCols(colNames, colTypes, na, has_col_names);
}
