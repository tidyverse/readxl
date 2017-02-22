#ifndef READXL_XLSCELL_
#define READXL_XLSCELL_

#include <Rcpp.h>
#include <libxls/xls.h>
#include <libxls/xlstypes.h>
#include "CellType.h"

class XlsCell {
  xls::xlsCell *cell_;
  std::pair<int,int> location_;

public:

  XlsCell(xls::xlsCell *cell): cell_(cell) {
    location_ = std::make_pair(cell_->row, cell_->col);
  }

  int row() const {
    return location_.first;
  }

  int col() const {
    return location_.second;
  }

};
#endif
