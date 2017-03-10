#include <Rcpp.h>
#include "XlsxWorkSheet.h"
#include "ColSpec.h"
#include "utils.h"
using namespace Rcpp;

// [[Rcpp::export]]
IntegerVector xlsx_dim(std::string path, int sheet_i = 0, int skip = 0) {
  XlsxWorkSheet ws(path, sheet_i, skip);
  return IntegerVector::create(ws.nrow(), ws.ncol());
}

// [[Rcpp::export]]
IntegerVector parse_ref(std::string ref) {
  std::pair<int,int> parsed = parseRef(ref.c_str());

  return IntegerVector::create(parsed.first, parsed.second);
}

// [[Rcpp::export]]
CharacterVector xlsx_col_types(std::string path, int sheet_i = 0,
                               CharacterVector na = CharacterVector(),
                               int skip = 0, int guess_max = 1000,
                               bool has_col_names = false) {

  XlsxWorkSheet ws(path, sheet_i, skip);
  std::vector<ColType> types(ws.ncol());
  std::fill(types.begin(), types.end(), COL_UNKNOWN);
  types = ws.colTypes(types, na, guess_max, has_col_names);
  return colTypeDescs(types);
}

// [[Rcpp::export]]
CharacterVector xlsx_col_names(std::string path,
                               CharacterVector na = CharacterVector(),
                               int sheet_i = 0, int skip = 0) {
  return XlsxWorkSheet(path, sheet_i, skip).colNames(na);
}

// [[Rcpp::export]]
List read_xlsx_(std::string path, int sheet_i, RObject col_names,
                RObject col_types, std::vector<std::string> na,
                int skip = 0, int guess_max = 1000) {

  XlsxWorkSheet ws(path, sheet_i, skip);

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
