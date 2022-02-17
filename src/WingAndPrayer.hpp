#include "CellLimits.h"
#include "ColSpec.h"
#include "XlsWorkBook.h"
#include "XlsxWorkBook.h"
#include "XlsSheetData.h"
#include "XlsxSheetData.h"
#include "XlsCell.h"
#include "XlsxCell.h"

#include "cpp11/R.hpp"
#include "cpp11/strings.hpp"

#include <algorithm>
#include <string>
#include <vector>

class Xls {
public:
 typedef XlsWorkBook  Book;
 typedef XlsSheetData SheetData;
 typedef XlsCell      Cell;
};

class Xlsx {
public:
  typedef XlsxWorkBook  Book;
  typedef XlsxSheetData SheetData;
  typedef XlsxCell      Cell;
};

template <typename T>
class Sheet {

public:
  Sheet(const std::string& path,
        int sheet_i, cpp11::integers limits, bool shim, bool progress)
    : wb_(path),
      sd_(wb_, sheet_i, limits, shim, progress)
  {
    Rprintf("Sheet() constructor\n");
    Rprintf("Reading from: %s\n", wb_.path().c_str());
    Rprintf(
      "Reading %d rows x %d cols from worksheet '%s'\n",
      sd_.nrow(), sd_.ncol(), sd_.sheetName().c_str());
  }

  int ncol() const { return sd_.ncol(); }
  int nrow() const { return sd_.nrow(); }

  cpp11::strings colNames(const StringSet &na, const bool trimWs) {
    cpp11::writable::strings out(sd_.ncol());
    typename std::vector<typename T::Cell>::iterator xcell = sd_.cells_.begin();
    int base = xcell->row();

    while(xcell != sd_.cells_.end() && xcell->row() == base) {
      xcell->inferType(na, trimWs, wb_.dateFormats(), wb_.stringTable());
      int position = xcell->col() - sd_.startCol();
      out[position] = cpp11::r_string(xcell->asCharSxp(trimWs, wb_.stringTable()));
      xcell++;
    }
    return out;
  }

  std::vector<ColType> colTypes(std::vector<ColType> types,
                                const StringSet &na,
                                const bool trimWs,
                                int guess_max = 1000,
                                bool has_col_names = false) {
    if (guess_max == 0) {
      return types;
    }

    typename std::vector<typename T::Cell>::iterator xcell;
    xcell = has_col_names ? advance_row(sd_.cells_) : sd_.cells_.begin();

    // no cell data to consult re: types
    if (xcell == sd_.cells_.end()) {
      std::fill(types.begin(), types.end(), COL_BLANK);
      return types;
    }

    std::vector<bool> type_known(types.size());
    for (size_t j = 0; j < types.size(); j++) {
      type_known[j] = types[j] != COL_UNKNOWN;
    }

    // count is for spinner and checking for interrupt
    int count = 0;
    // base is row the data starts on **in the spreadsheet**
    int base = sd_.cells_.begin()->row() + has_col_names;
    while (xcell != sd_.cells_.end() && xcell->row() - base < guess_max) {
      count++;
      if (count % PROGRESS_TICK == 0) {
        //spinner_.spin();
        cpp11::check_user_interrupt();
      }
      int j = xcell->col() - sd_.startCol();
      if (type_known[j] || types[j] == COL_TEXT) {
        xcell++;
        continue;
      }
      xcell->inferType(na, trimWs, wb_.dateFormats(), wb_.stringTable());
      ColType type = as_ColType(xcell->type());
      if (type > types[j]) {
        types[j] = type;
      }
      xcell++;
    }

    return types;
  }

  cpp11::list readCols(cpp11::strings names,
                       const std::vector<ColType>& types,
                       const StringSet &na, const bool trimWs,
                       bool has_col_names = false) {

    typename std::vector<typename T::Cell>::iterator xcell;
    xcell = has_col_names ? advance_row(sd_.cells_) : sd_.cells_.begin();

    // base is row the data starts on **in the spreadsheet**
    int base = sd_.cells_.begin()->row() + has_col_names;
    int n = (xcell == sd_.cells_.end()) ? 0 : sd_.lastRow() - base + 1;
    cpp11::writable::list cols(sd_.ncol());
    cols.attr("names") = names;
    for (int j = 0; j < sd_.ncol(); ++j) {
      cols[j] = makeCol(types[j], n);
    }

    if (n == 0) {
      return cols;
    }

    // count is for spinner and checking for interrupt
    int count = 0;
    while (xcell != sd_.cells_.end()) {

      int i = xcell->row();
      int j = xcell->col();
      // col to write into
      int col = j - sd_.startCol();

      count++;
      if (count % PROGRESS_TICK == 0) {
        //spinner_.spin();
        cpp11::check_user_interrupt();
      }

      if (types[col] == COL_SKIP) {
        xcell++;
        continue;
      }

      xcell->inferType(na, trimWs, wb_.dateFormats(), wb_.stringTable());
      CellType type = xcell->type();
      cpp11::sexp column = cpp11::as_sexp(cols[col]);
      // row to write into
      int row = i - base;

      // Fit cell of type x into a column of type y
      // Conventions:
      //   * process type in same order as enum, unless reason to do otherwise
      //   * access cell contents only via asWhatever() methods
      switch(types[col]) {

      case COL_UNKNOWN:
      case COL_BLANK:
      case COL_SKIP:
        break;

      case COL_LOGICAL:
        if (type == CELL_DATE) {
          // print date string here, when/if it's possible to do so
          cpp11::warning("Expecting logical in %s: got a date",
                         cellPosition(i, j).c_str());
        }

        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
        case CELL_LOGICAL:
        case CELL_DATE:
        case CELL_NUMERIC:
          LOGICAL(column)[row] = xcell->asLogical();
          break;
        case CELL_TEXT: {
          std::string text_string = xcell->asStdString(trimWs, wb_.stringTable());
          bool text_boolean;
          if (logicalFromString(text_string, &text_boolean)) {
            LOGICAL(column)[row] = text_boolean;
          } else {
            cpp11::warning("Expecting logical in %s: got '%s'",
                           cellPosition(i, j).c_str(), text_string.c_str());
            LOGICAL(column)[row] = NA_LOGICAL;
          }
        }
          break;
        }
        break;

      case COL_DATE:
        if (type == CELL_LOGICAL) {
          cpp11::warning("Expecting date in %s: got boolean", cellPosition(i, j).c_str());
        }
        if (type == CELL_NUMERIC) {
          cpp11::warning("Coercing numeric to date in %s",
                         cellPosition(i, j).c_str());
        }
        if (type == CELL_TEXT) {
          cpp11::warning("Expecting date in %s: got '%s'", cellPosition(i, j).c_str(),
                         (xcell->asStdString(trimWs, wb_.stringTable())).c_str());
        }
        REAL(column)[row] = xcell->asDate(wb_.is1904());
        break;

      case COL_NUMERIC:
        if (type == CELL_LOGICAL) {
          cpp11::warning("Coercing boolean to numeric in %s", cellPosition(i, j).c_str());
        }
        if (type == CELL_DATE) {
          // print date string here, when/if possible
          cpp11::warning("Expecting numeric in %s: got a date", cellPosition(i, j).c_str());
        }
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
        case CELL_LOGICAL:
        case CELL_DATE:
        case CELL_NUMERIC:
          REAL(column)[row] = xcell->asDouble();
          break;
        case CELL_TEXT: {
          std::string num_string = xcell->asStdString(trimWs, wb_.stringTable());
          double num_num;
          bool success = doubleFromString(num_string, num_num);
          if (success) {
            cpp11::warning("Coercing text to numeric in %s: '%s'",
                           cellPosition(i, j).c_str(), num_string.c_str());
            REAL(column)[row] = num_num;
          } else {
            cpp11::warning("Expecting numeric in %s: got '%s'",
                           cellPosition(i, j).c_str(), num_string.c_str());
            REAL(column)[row] = NA_REAL;
          }
        }
          break;
        }
        break;

      case COL_TEXT:
        // not issuing warnings for NAs or coercion, because "text" is the
        // fallback column type and there are too many warnings to be helpful
        SET_STRING_ELT(column, row, xcell->asCharSxp(trimWs, wb_.stringTable()));
        break;

      case COL_LIST:
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
          SET_VECTOR_ELT(column, row, Rf_ScalarLogical(NA_LOGICAL));
          break;
        case CELL_LOGICAL:
          SET_VECTOR_ELT(column, row, Rf_ScalarLogical(xcell->asLogical()));
          break;
        case CELL_DATE: {
          cpp11::sexp cell_val = Rf_ScalarReal(xcell->asDate(wb_.is1904()));
          cell_val.attr("class") = {"POSIXct", "POSIXt"};
          cell_val.attr("tzone") = "UTC";
          SET_VECTOR_ELT(column, row, cell_val);
          break;
        }
        case CELL_NUMERIC:
          SET_VECTOR_ELT(column, row, Rf_ScalarReal(xcell->asDouble()));
          break;
        case CELL_TEXT: {
          cpp11::writable::strings rStringVector({NA_STRING});
          SET_STRING_ELT(rStringVector, 0, xcell->asCharSxp(trimWs, wb_.stringTable()));
          SET_VECTOR_ELT(column, row, rStringVector);
          break;
        }
        }
      }
      xcell++;
    }

    return removeSkippedColumns(cols, names, types);
  }

private:
  typename T::Book wb_;
  typename T::SheetData sd_;
};
