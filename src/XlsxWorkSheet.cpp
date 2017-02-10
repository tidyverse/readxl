#include <Rcpp.h>
#include "XlsxWorkSheet.h"
using namespace Rcpp;

// [[Rcpp::export]]
IntegerVector xlsx_dim(std::string path, int sheet = 0, int nskip = 0) {
  XlsxWorkSheet ws(path, sheet, nskip);
  return IntegerVector::create(ws.nrow(), ws.ncol());
}

// [[Rcpp::export]]
IntegerVector parse_ref(std::string ref) {
  std::pair<int,int> parsed = parseRef(ref.c_str());

  return IntegerVector::create(parsed.first, parsed.second);
}

// [[Rcpp::export]]
CharacterVector xlsx_col_types(std::string path, int sheet = 0,
                               CharacterVector na = CharacterVector(),
                               int nskip = 0, int guess_max = 1000) {

  XlsxWorkSheet ws(path, sheet, nskip);
  std::vector<CellType> types = ws.colTypes(na, nskip, guess_max);

  CharacterVector out(types.size());
  for (size_t i = 0; i < types.size(); ++i) {
    out[i] = cellTypeDesc(types[i]);
  }

  return out;
}

// [[Rcpp::export]]
CharacterVector xlsx_col_names(std::string path, int sheet = 0, int nskip = 0) {
  return XlsxWorkSheet(path, sheet, nskip).colNames();
}

// [[Rcpp::export]]
List read_xlsx_(std::string path, int sheet, RObject col_names,
                RObject col_types, std::vector<std::string> na,
                int nskip = 0, int guess_max = 1000) {

  XlsxWorkSheet ws(path, sheet, nskip);

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
  std::vector<CellType> colTypes;
  switch(TYPEOF(col_types)) {
  case NILSXP:
    colTypes = ws.colTypes(na, guess_max, sheetHasColumnNames);
    break;
  case STRSXP:
    colTypes = cellTypes(as<CharacterVector>(col_types));
    break;
  default:
    Rcpp::stop("`col_types` must be a character vector or NULL");
  }

  if (sheetHasColumnNames) {
    // convert blank columns that have a name to numeric
    for (size_t i = 0; i < colTypes.size(); i++) {
      if (colTypes[i] == CELL_BLANK &&
          colNames[i] != NA_STRING &&
          colNames[i] != "")
        colTypes[i] = CELL_NUMERIC;
    }
  }

  return ws.readCols(colNames, colTypes, na, sheetHasColumnNames);
}
