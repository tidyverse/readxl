#include <Rcpp.h>
#include "XlsxWorkSheet.h"
using namespace Rcpp;

// [[Rcpp::export]]
void xlsx_cells(std::string path, int sheet = 0) {
  return XlsxWorkSheet(path, sheet).printCells();
}

// [[Rcpp::export]]
IntegerVector xlsx_dim(std::string path, int sheet = 0) {
  XlsxWorkSheet ws(path, sheet);
  return IntegerVector::create(ws.nrow(), ws.ncol());
}

// [[Rcpp::export]]
IntegerVector parse_ref(std::string ref) {
  std::pair<int,int> parsed = parseRef(ref.c_str());

  return IntegerVector::create(parsed.first, parsed.second);
}

// [[Rcpp::export]]
CharacterVector xlsx_col_types(std::string path, int sheet = 0,
                               CharacterVector na = CharacterVector(), int nskip = 0,
                               int n = 100) {

  XlsxWorkSheet ws(path, sheet);
  std::vector<CellType> types = ws.colTypes(na, nskip, n);

  CharacterVector out(types.size());
  for (size_t i = 0; i < types.size(); ++i) {
    out[i] = cellTypeDesc(types[i]);
  }

  return out;
}

// [[Rcpp::export]]
CharacterVector xlsx_col_names(std::string path, int sheet = 0, int nskip = 0) {
  return XlsxWorkSheet(path, sheet).colNames(nskip);
}

// [[Rcpp::export]]
List read_xlsx_(std::string path, int sheet, RObject col_names,
                RObject col_types, std::vector<std::string> na, int nskip = 0) {

  XlsxWorkSheet ws(path, sheet);

  if (ws.nrow() == 0 && ws.ncol() == 0) {
    return Rcpp::List(0);
  }

  // Standardise column names --------------------------------------------------
  CharacterVector colNames;
  bool sheetHasColumnNames = false;
  switch(TYPEOF(col_names)) {
  case STRSXP:
    colNames = as<CharacterVector>(col_names);
    break;
  case LGLSXP:
  {
    sheetHasColumnNames = as<bool>(col_names);
    if (sheetHasColumnNames) {
      colNames = ws.colNames(std::max(ws.min_row(), nskip));
    } else {
      int p = ws.ncol();
      colNames = CharacterVector(p);
      for (int j = 0; j < p; ++j) {
        colNames[j] = tfm::format("X%i", j+1);
      }
    }
    break;
  }
  default:
    Rcpp::stop("`col_names` must be a logical or character vector");
  }

  // if we know there's no data reading to be done, we should return a
  // 0 row tibble HERE with the correct colNames
  // TO THINK: case where colTypes were explicitly specified but no data to read

  // Standardise column types --------------------------------------------------
  std::vector<CellType> colTypes;
  switch(TYPEOF(col_types)) {
  case NILSXP:
    colTypes = ws.colTypes(na, std::max(ws.min_row(), nskip),
                           100, sheetHasColumnNames);
    break;
  case STRSXP:
    colTypes = cellTypes(as<CharacterVector>(col_types));
    break;
  default:
    Rcpp::stop("`col_types` must be a character vector or NULL");
  }

  return ws.readCols(colNames, colTypes, na,
                     std::max(ws.min_row(), nskip), sheetHasColumnNames);
}
