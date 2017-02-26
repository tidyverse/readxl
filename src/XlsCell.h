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
  // [MS-XLS] - v20161017, Release: October 17, 2016
  //
  // In 2.2.1 Cell Table on p80:
  // "Cells are specified by any of the records specified in the CELL rule."
  // (section 2.1.7.20.6).
  // In 2.1.7.20.6 on p74, here is the CELL rule:
  // CELL = FORMULA / Blank / MulBlank / RK / MulRk / BoolErr / Number / LabelSst
  //
  // 2.3 Record Enumeration
  // Has 2 tables associating each record type value with a name and number.
  // 2.3.1 starting p168 is ordered by name
  // 2.3.2 starting p180 is ordered by number
  //
  // See xls_addCell for those used for cells
  // and xlsstruct.h to confirm record numbers
  switch(cell.id) {
  case 253: // 0x00FD LabelSst
  case 516: // 0x0204 Label
    return na.contains((char*) cell.str) ? CELL_BLANK : CELL_TEXT;
    break;

  case 6:    // 0x0006 formula
  case 1030: // 0x0406 formula (Apple Numbers Bug)
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

  case 189: // 0x00BD MulRk
  case 515: // 0x0203 Number
  case 638: // 0x027E Rk
    {
      if (na.contains(cell.d))
        return CELL_BLANK;

      if (styles == NULL)
        return CELL_NUMERIC;

      int format = styles->xf[cell.xf].format;
      return isDateTime(format, customDateFormats) ? CELL_DATE : CELL_NUMERIC;
    }
    break;

  case 190: // 0x00BE MulBlank
  case 513: // 0x0201 Blank
    return CELL_BLANK;
    break;

  case 517: // 0x0205 BoolErr
    {
      if (!strcmp((char *) cell.str, "bool")) {
        // switch to CELL_LOGICAL once exists; cell.d is 0/1 for FALSE/TRUE
        return CELL_NUMERIC;
      } else {
        return CELL_TEXT;
      }
    }
    break;

  default:
    Rcpp::Rcout << "Unknown type: " << cell.id << "\n";
  return CELL_NUMERIC;
  }
}

#endif
