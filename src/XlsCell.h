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

  CellType type(const StringSet& na,
                const xls::st_xf* styles,
                const std::set<int>& customDateFormats) const {

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
    switch(cell_->id) {
    case 253: // 0x00FD LabelSst 2.4.149 p325:
              // a string from the shared string table
    case 516: // 0x0204 Label 2.4.148 p325:
              // "Label record specifies a label on the category axis for
              // each series"
              // Jenny: I think this one is a red herring = not a cell type
      return na.contains((char*) cell_->str) ? CELL_BLANK : CELL_TEXT;

    case 6:    // 0x0006 formula 2.4.127 p309
    case 1030: // 0x0406 formula (Apple Numbers Bug) via libxls
      if (cell_->l == 0) { // formula evaluates to numeric, possibly date
        if (na.contains(cell_->d)) {
          return CELL_BLANK;
        }
        int format = styles->xf[cell_->xf].format;
        return isDateTime(format, customDateFormats) ? CELL_DATE : CELL_NUMERIC;
      } else { // formula evaluates to Boolean, string, or error
        if (!strcmp((char *) cell_->str, "bool")) {
          return CELL_LOGICAL;
        } else if (na.contains((char*) cell_->str)) {
          return CELL_BLANK;
        } else {
          // error should become NA
          return cell_->d == 0 ? CELL_TEXT : CELL_BLANK;
        }
      }

    case 189: // 0x00BD MulRk 2.4.175 p344:
              // numeric data originating from series of cells
    case 515: // 0x0203 Number 2.4.180 p348:
              // floating-point number from single cell
    case 638: // 0x027E Rk 2.4.220 p376:
              // numeric data from single cell
      {
        if (na.contains(cell_->d)) {
          return CELL_BLANK;
        }

        if (styles == NULL) {
          return CELL_NUMERIC;
        }

        int format = styles->xf[cell_->xf].format;
        return isDateTime(format, customDateFormats) ? CELL_DATE : CELL_NUMERIC;
      }

    case 190: // 0x00BE MulBlank 2.4.174 p344:
              // blank cell originating from series of blank cells
    case 513: // 0x0201 Blank 2.4.20 p212:
              // an empty cell with no formula or value
      return CELL_BLANK;

    case 517: // 0x0205 BoolErr 2.4.24 p216:
              //  a cell that contains either a Boolean value or an error value
      if (na.contains((char*) cell_->str)) {
        return CELL_BLANK;
      }
      // error should become NA
      // 2.5.10 Bes p592 explains the codes, if we ever change our minds
      return (!strcmp((char *) cell_->str, "bool")) ? CELL_LOGICAL : CELL_BLANK;

    default:
      Rcpp::warning("Unrecognized cell type at [%i, %i]: '%s'",
                    row() + 1, col() + 1, cell_->id);
    }

    return CELL_TEXT;
  }

};

#endif
