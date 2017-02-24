#ifndef READXL_XLSCELL_
#define READXL_XLSCELL_

#include <Rcpp.h>
#include <libxls/xls.h>
#include <libxls/xlstypes.h>
#include "ColSpec.h"

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

  xls::xlsCell* cell() const {
    return cell_;
  }

};

inline CellType cellType(const xls::st_cell::st_cell_data cell,
                         xls::st_xf* styles,
                         const std::set<int>& customDateFormats,
                         const StringSet &na = "") {
  // Find codes in [MS-XLS] S2.3.2 (p175).
  // See xls_addCell for those used for cells
  switch(cell.id) {
  case 253: // LabelSst
  case 516: // Label
    return na.contains((char*) cell.str) ? CELL_BLANK : CELL_TEXT;
    break;

  case 6:    // formula
  case 1030: // formula (Apple Numbers Bug)
    if (cell.l == 0) {
      return na.contains(cell.d) ? CELL_BLANK : CELL_NUMERIC;
    } else {
      if (na.contains((char*) cell.str)) {
        return CELL_BLANK;
      } else {
        return CELL_TEXT;
      }
    }
    break;

  case 189: // MulRk
  case 515: // Number
  case 638: // Rk
    {
      if (na.contains(cell.d))
        return CELL_BLANK;

      if (styles == NULL)
        return CELL_NUMERIC;

      int format = styles->xf[cell.xf].format;
      return isDateTime(format, customDateFormats) ? CELL_DATE : CELL_NUMERIC;
    }
    break;

  case 190: // MulBlank
  case 513: // Blank
    return CELL_BLANK;
    break;

  default:
    Rcpp::Rcout << "Unknown type: " << cell.id << "\n";
  return CELL_NUMERIC;
  }
}

#endif
