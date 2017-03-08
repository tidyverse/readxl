#ifndef READXL_XLSCELL_
#define READXL_XLSCELL_

#include <Rcpp.h>
#include <libxls/xls.h>
#include <libxls/xlstypes.h>
#include "ColSpec.h"

// Key reference for understanding the structure of the xls format is
// [MS-XLS]: Excel Binary File Format (.xls) Structure
// https://msdn.microsoft.com/en-us/library/cc313154(v=office.12).aspx
// http://interoperability.blob.core.windows.net/files/MS-XLS/[MS-XLS].pdf
// Page and section numbers below refer to
// v20161017, Release: October 17, 2016

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
    // 1. Review of Excel's declared cell types, then
    // 2. Summary of how Excel's cell types map to our CellType enum
    //
    // 2.2.1 Cell Table p80:
    // "Cells are specified by any of the records specified in the CELL rule
    // (section 2.1.7.20.6)."
    //
    // 2.1.7.20.6 p74 The CELL Rule:
    // CELL = FORMULA / Blank / MulBlank / RK / MulRk / BoolErr / Number / LabelSst
    //
    // 2.3 Record Enumeration
    // Has 2 tables associating each record type value with a name and number.
    // 2.3.1 starting p168 is ordered by name
    // 2.3.2 starting p180 is ordered by number
    //
    // For info on how libxls extracts and exposes, see xls_addCell. Also
    // consult xlsstruct.h to confirm record numbers.
    //
    // We map Excel's cell types to the CellType enum based on declared type
    // and contents.
    //
    // CELL_BLANK
    //   shared string or string formula whose value matches na
    //   boolean or boolean formula whose value (TRUE or FALSE) matches na
    //   number or numeric formula whose double value (d) matches na
    //   explicit blank cell
    //   formula in error (except #NULL!) or static error (#N/A)
    //   examples:
    //   static: id = 517, str = "error", d = 42, l = 0, xf = 15
    //   formula: id = 6, str = "error", d = 29, l = 65535, xf = 15
    //
    // CELL_LOGICAL
    //   boolean or boolean formula whose value (TRUE or FALSE) does not match na
    //   examples:
    //   static: id = 517, str = "bool", d = 0 or 1, l = 0, xf = 15
    //   formula: id = 6, str = "bool", d = 0 or 1, l = 65535, xf = 15
    //
    // CELL_DATE
    //   number or numeric formula with a date format, whose double value (d)
    //   does not match na
    //   examples:
    //   static: id = 638, str = "42426.000000", d = 42426, l = 0, xf = 62
    //   formula: id = 6, str = "42431", d = 42431, l = 0, xf = 62
    //
    // CELL_NUMERIC
    //   number or numeric formula with no format or a non-date format, whose
    //   double value (d) does not match na
    //   examples:
    //   static: id = 638, str = "1.300000", d = 1.3, l = 0, xf = 15
    //   formula: id = 6, str = "1", d = 1.69, l = 0, xf = 15
    //
    // CELL_TEXT
    //   shared string or string formula whose value does not match na
    //   examples:
    //   static: id = 253, str = "something", d = 0, l = 0, xf = 15
    //   formula: id = 6, str = "something", d = 0, l = 65535, xf = 15

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
      // l = 0     --> result is a number, possibly date
      // l = 65535 --> everything else
      if (cell_->l == 0) {
        if (na.contains(cell_->d)) {
          return CELL_BLANK;
        }
        if (styles == NULL) {
          return CELL_NUMERIC;
        }
        int format = styles->xf[cell_->xf].format;
        return isDateTime(format, customDateFormats) ? CELL_DATE : CELL_NUMERIC;
      } else { // formula evaluates to Boolean, string, or error

        // Boolean
        if (strncmp((char *) cell_->str, "bool", 4) == 0) {
          if ( (cell_->d == 0 && na.contains("FALSE")) ||
               (cell_->d == 1 && na.contains("TRUE")) ) {
            return CELL_BLANK;
          } else {
            return CELL_LOGICAL;
          }
        }

        // error
        // libxls puts "error" in str for all errors and
        // puts the error code in d
        //  Code Error
        //  0x00 #NULL! <-- indistinguishable from "error" formula string :(
        //  0x07 #DIV/0!
        //  0x0F #VALUE!
        //  0x17 #REF!
        //  0x1D #NAME?
        //  0x24 #NUM!
        //  0x2A #N/A
        //  0x2B #GETTING_DATA
        if (strncmp((char *) cell_->str, "error", 5) == 0 && cell_->d > 0) {
          return CELL_BLANK;
        }

        // string (or #NULL! error)
        // d = 0 and str holds string formula result
        return na.contains((char*) cell_->str) ? CELL_BLANK : CELL_TEXT;
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
      if (strncmp((char *) cell_->str, "bool", 4) == 0) {
        if ( (cell_->d == 0 && na.contains("FALSE")) ||
             (cell_->d == 1 && na.contains("TRUE")) ) {
          return CELL_BLANK;
        } else {
          return CELL_LOGICAL;
        }
      }
      // must be an error
      return CELL_BLANK;

    default:
      Rcpp::warning("Unrecognized cell type at [%i, %i]: '%s'",
                    row() + 1, col() + 1, cell_->id);
    }

    return CELL_TEXT;
  }

  std::string asStdString(const StringSet& na,
                          const xls::st_xf* styles,
                          const std::set<int>& customDateFormats) const {
    CellType type = this->type(na, styles, customDateFormats);

    switch(type) {

    case CELL_UNKNOWN:
    case CELL_BLANK:
      return "";

    case CELL_LOGICAL:
      return cell_->d ? "TRUE" : "FALSE";

    case CELL_DATE:
      // not ideal for a date but will have to do ... one day: asDateString()?
    case CELL_NUMERIC: {
      std::ostringstream strs;
      strs << cell_->d;
      std::string out_string = strs.str();
      return out_string;
    }

    case CELL_TEXT:
      return std::string((char*) cell_->str);
  }
  }

  Rcpp::RObject asCharSxp(const StringSet& na,
                          const xls::st_xf* styles,
                          const std::set<int>& customDateFormats) const {
    std::string out_string = asStdString(na, styles, customDateFormats);
    return out_string.empty() ? NA_STRING : Rf_mkCharCE(out_string.c_str(), CE_UTF8);
  }

};

#endif
