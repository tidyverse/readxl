#include <Rcpp.h>
#include "ColSpec.h"
#include "XlsWorkSheet.h"
#include <libxls/xls.h>
using namespace Rcpp;

// [[Rcpp::export]]
List read_xls_(std::string path, int sheet_i,
               IntegerVector limits, bool shim,
               RObject col_names, RObject col_types,
               std::vector<std::string> na, bool trim_ws,
               int guess_max = 1000, bool progress = true) {

  // Construct worksheet ----------------------------------------------
  XlsWorkSheet ws(path, sheet_i, limits, shim, progress);

  // catches empty sheets and sheets where requested rectangle contains no data
  if (ws.nrow() == 0 && ws.ncol() == 0) {
    return Rcpp::List(0);
  }

  // Get column names -------------------------------------------------
  CharacterVector colNames;
  bool has_col_names = false;
  switch(TYPEOF(col_names)) {
  case STRSXP:
    colNames = as<CharacterVector>(col_names);
    break;
  case LGLSXP:
    has_col_names = as<bool>(col_names);
    colNames = has_col_names ? ws.colNames(na, trim_ws) : CharacterVector(ws.ncol(), "");
    break;
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
    colTypes = ws.colTypes(colTypes, na, trim_ws, guess_max, has_col_names);
  }
  colTypes = finalizeTypes(colTypes);

  // Reconcile column names and types ----------------------------------
  colNames = reconcileNames(colNames, colTypes, sheet_i);

  // Get data ----------------------------------------------------------
  return ws.readCols(colNames, colTypes, na, trim_ws, has_col_names);
}
