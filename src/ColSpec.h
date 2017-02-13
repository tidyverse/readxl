#ifndef READXL_COLSPEC_
#define READXL_COLSPEC_

#include <Rcpp.h>
#include "CellType.h"

inline Rcpp::CharacterVector reconcileNames(Rcpp::CharacterVector names,
                                            std::vector<CellType> types,
                                            int sheet) {
  size_t ncol_names = names.size();
  size_t ncol_types = types.size();

  if (ncol_names == ncol_types) {
    return names;
  }

  size_t ncol_noskip = 0;
  for (size_t i = 0; i < types.size(); i++) {
    if (types[i] != CELL_SKIP) {
      ncol_noskip++;
    }
  }
  if (ncol_names != ncol_noskip) {
    Rcpp::stop("Sheet %d has %d columns (%d unskipped), but `col_names` has length %d.",
               sheet + 1, ncol_types, ncol_noskip, ncol_names);
  }

  Rcpp::CharacterVector newNames(ncol_types, "");
  size_t j_short = 0;
  for (size_t j_long = 0; j_long < ncol_types; ++j_long) {
    if (types[j_long] == CELL_SKIP) {
      continue;
    }
    newNames[j_long] = names[j_short];
    j_short++;
  }
  return newNames;
}

#endif
