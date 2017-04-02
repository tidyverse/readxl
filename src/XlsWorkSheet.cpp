#include <Rcpp.h>

#include "XlsWorkSheet.h"
#include "ColSpec.h"
#include <libxls/xls.h>
using namespace Rcpp;

// [[Rcpp::export]]
List read_xls_(std::string path, int sheet_i,
               RObject input_limits, RObject input_shrink,
               RObject col_names, RObject col_types,
               std::vector<std::string> na, int guess_max = 1000) {

  // fix this!
  // maybe I should use a map?
  IntegerVector limits = as<IntegerVector>(input_limits);
  bool shrink = as<bool>(input_shrink);

  // Construct worksheet ----------------------------------------------
  XlsWorkSheet ws(path, sheet_i, limits, shrink);

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
    colNames = has_col_names ? ws.colNames(na) : CharacterVector(ws.ncol(), "");
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
    colTypes = ws.colTypes(colTypes, na, guess_max, has_col_names);
  }
  colTypes = finalizeTypes(colTypes);

  // Reconcile column names and types ----------------------------------
  colNames = reconcileNames(colNames, colTypes, sheet_i);

  // Get data ----------------------------------------------------------
  return ws.readCols(colNames, colTypes, na, has_col_names);
}
