#ifndef READXL_XLSWORKSHEET_
#define READXL_XLSWORKSHEET_

#include "CellLimits.h"
#include "Spinner.h"
#include "XlsCell.h"
#include "XlsWorkBook.h"

#include "libxls/xls.h"
#include "libxls/xlsstruct.h"

#include "cpp11/as.hpp"
#include "cpp11/integers.hpp"
#include "cpp11/list.hpp"
#include "cpp11/protect.hpp"
#include "cpp11/sexp.hpp"
#include "cpp11/strings.hpp"

const int PROGRESS_TICK = 131072; // 2^17

class XlsWorkSheet {
  // the host workbook
  XlsWorkBook wb_;

  // xls specifics
  xls::xlsWorkBook* pWB_;
  xls::xlsWorkSheet* pWS_;

  // common to xls[x]
  std::set<int> dateFormats_;
  std::vector<XlsCell> cells_;
  std::string sheetName_;
  CellLimits nominal_, actual_;
  int ncol_, nrow_;
  Spinner spinner_;

public:

  XlsWorkSheet(const XlsWorkBook wb, int sheet_i,
               cpp11::integers limits, bool shim, bool progress):
  wb_(wb), nominal_(limits), spinner_(progress)
  {
    if (sheet_i >= wb.n_sheets()) {
      cpp11::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                  sheet_i + 1, wb.n_sheets());
    }
    sheetName_ = cpp11::r_string(wb.sheets()[sheet_i]);

    xls::xls_error_t error = xls::LIBXLS_OK;
    std::string path = wb_.path();
    spinner_.spin();
    pWB_ = xls_open_file(path.c_str(), "UTF-8", &error);
    if (!pWB_) {
      cpp11::stop(
        "\n  filepath: %s\n  libxls error: %s",
        path.c_str(),
        xls::xls_getError(error)
      );
    }
    spinner_.spin();

    pWS_ = xls::xls_getWorkSheet(pWB_, sheet_i);
    if (pWS_ == NULL) {
      cpp11::stop("Sheet '%s' (position %d): cannot be opened",
                  sheetName_.c_str(), sheet_i + 1);
    }
    xls_parseWorkSheet(pWS_);
    spinner_.spin();
    dateFormats_ = wb.dateFormats();

    // nominal_ holds user's geometry request
    loadCells(shim);
    // nominal_ may have been shifted (case of implicit skipping and n_max)
    // actual_ reports populated cells inside the nominal_ rectangle

    // insert shims and update actual_
    if (shim) insertShims();

    nrow_ = (actual_.minRow() < 0) ? 0 : actual_.maxRow() - actual_.minRow() + 1;
    ncol_ = (actual_.minCol() < 0) ? 0 : actual_.maxCol() - actual_.minCol() + 1;
  }

  ~XlsWorkSheet() {
    try {
      xls::xls_close_WS(pWS_);
      xls::xls_close_WB(pWB_);
    } catch(...) {}
  }

  int ncol() const {
    return ncol_;
  }

  int nrow() const {
    return nrow_;
  }

  cpp11::strings colNames(const StringSet &na, const bool trimWs) {
    cpp11::writable::strings out(ncol_);
    std::vector<XlsCell>::iterator xcell = cells_.begin();
    int base = xcell->row();

    while(xcell != cells_.end() && xcell->row() == base) {
      xcell->inferType(na, trimWs, dateFormats_);
      int position = xcell->col() - actual_.minCol();
      out[position] = cpp11::r_string(xcell->asCharSxp(trimWs));
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

    std::vector<XlsCell>::iterator xcell;
    xcell = has_col_names ? advance_row(cells_) : cells_.begin();

    // no cell data to consult re: types
    if (xcell == cells_.end()) {
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
    int base = cells_.begin()->row() + has_col_names;
    while (xcell != cells_.end() && xcell->row() - base < guess_max) {
      count++;
      if (count % PROGRESS_TICK == 0) {
        spinner_.spin();
        cpp11::check_user_interrupt();
      }
      int j = xcell->col() - actual_.minCol();
      if (type_known[j] || types[j] == COL_TEXT) {
        xcell++;
        continue;
      }
      xcell->inferType(na, trimWs, dateFormats_);
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

    std::vector<XlsCell>::iterator xcell;
    xcell = has_col_names ? advance_row(cells_) : cells_.begin();

    // base is row the data starts on **in the spreadsheet**
    int base = cells_.begin()->row() + has_col_names;
    int n = (xcell == cells_.end()) ? 0 : actual_.maxRow() - base + 1;
    cpp11::writable::list cols(ncol_);
    cols.attr("names") = names;
    for (int j = 0; j < ncol_; ++j) {
      cols[j] = makeCol(types[j], n);
    }

    if (n == 0) {
      return cols;
    }

    // count is for spinner and checking for interrupt
    int count = 0;
    while (xcell != cells_.end()) {

      int i = xcell->row();
      int j = xcell->col();
      // col to write into
      int col = j - actual_.minCol();

      count++;
      if (count % PROGRESS_TICK == 0) {
        spinner_.spin();
        cpp11::check_user_interrupt();
      }

      if (types[col] == COL_SKIP) {
        xcell++;
        continue;
      }

      xcell->inferType(na, trimWs, dateFormats_);
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
          std::string text_string = xcell->asStdString(trimWs);
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
          cpp11::warning("Expecting date in %s: got '%s'",
                        cellPosition(i, j).c_str(),
                        (xcell->asStdString(trimWs)).c_str());
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
          std::string num_string = xcell->asStdString(trimWs);
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
        SET_STRING_ELT(column, row, xcell->asCharSxp(trimWs));
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
          SET_STRING_ELT(rStringVector, 0, xcell->asCharSxp(trimWs));
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

  void loadCells(const bool shim) {
    // by convention, min_row = -2 means 'read no data'
    if (nominal_.minRow() < -1) {
      return;
    }

    // count is for spinner and checking for interrupt
    int count = 0;

    int nominal_ncol = pWS_->rows.lastcol;
    int nominal_nrow = pWS_->rows.lastrow;

    xls::xlsCell *cell;
    bool nominal_needs_checking = !shim && nominal_.maxRow() >= 0;
    for (int i = 0; i <= nominal_nrow; ++i) {

      if (i < nominal_.minRow() ||
          (!nominal_needs_checking && !nominal_.contains(i))) {
        continue;
      }

      for (xls::WORD j = 0; j <= nominal_ncol; ++j) {
        count++;
        if (count % PROGRESS_TICK == 0) {
          spinner_.spin();
          cpp11::check_user_interrupt();
        }

        if (nominal_needs_checking) {
          cell = xls_cell(pWS_, (xls::WORD) i, j);
          if (cell_is_readable(cell)) {
            if (i > nominal_.minRow()) { // implicit skip
              nominal_.update(
                i, i + nominal_.maxRow() - nominal_.minRow(),
                nominal_.minCol(), nominal_.maxCol()
              );
            }
            nominal_needs_checking = false;
          }
        }

        if (nominal_.contains(i, j)) {
          cell = xls_cell(pWS_, (xls::WORD) i, j);
          if (cell_is_readable(cell)) {
            cells_.push_back(cell);
            actual_.update(i, j);
          }
        }
      }
    }
  }

  // shim = TRUE when user specifies geometry via `range`
  // shim = FALSE when user specifies no geometry or uses `skip` and `n_max`
  //
  // nominal_ reflects user's geometry request
  // actual_ reports populated cells inside the nominal_ rectangle
  //
  // When shim = FALSE, we shrink-wrap the data that falls inside
  // the nominal_ rectangle.
  //
  // When shim = TRUE, we may need to insert dummy cells to fill out
  // the nominal_rectangle.
  //
  // actual_ is updated to reflect the insertions
  void insertShims() {

    // no cells were loaded
    if (cells_.empty()) {
      return;
    }

    // Recall cell limits are -1 by convention if the limit is unspecified.
    // funny_*() functions account for that.

    // if nominal min row or col is less than actual,
    // add a shim cell to the front of cells_
    bool   shim_up = funny_lt(nominal_.minRow(), actual_.minRow());
    bool shim_left = funny_lt(nominal_.minCol(), actual_.minCol());
    if (shim_up || shim_left) {
      int ul_row = funny_min(nominal_.minRow(), actual_.minRow());
      int ul_col = funny_min(nominal_.minCol(), actual_.minCol());
      XlsCell ul_shim(std::make_pair(ul_row, ul_col));
      cells_.insert(cells_.begin(), ul_shim);
      actual_.update(ul_row, ul_col);
    }

    // if nominal max row or col is greater than actual,
    // add a shim cell to the back of cells_
    bool  shim_down = funny_gt(nominal_.maxRow(), actual_.maxRow());
    bool shim_right = funny_gt(nominal_.maxCol(), actual_.maxCol());
    if (shim_down || shim_right) {
      int lr_row = funny_max(nominal_.maxRow(), actual_.maxRow());
      int lr_col = funny_max(nominal_.maxCol(), actual_.maxCol());
      XlsCell lr_shim(std::make_pair(lr_row, lr_col));
      cells_.push_back(lr_shim);
      actual_.update(lr_row, lr_col);
    }
  }

  bool funny_lt(const int funny, const int val) {
    return (funny >= 0) && (funny < val);
  }

  bool funny_gt(const int funny, const int val) {
    return (funny >= 0) && (funny > val);
  }

  int funny_min(const int funny, const int val) {
    return funny_lt(funny, val) ? funny : val;
  }

  int funny_max(const int funny, const int val) {
    return funny_gt(funny, val) ? funny : val;
  }

  std::vector<XlsCell>::iterator advance_row(std::vector<XlsCell>& x) {
    std::vector<XlsCell>::iterator it = x.begin();
    while (it != x.end() && it->row() == x.begin()->row()) {
      ++it;
    }
    return(it);
  }

  // Dimensions reported by xls and cells contained in xls include blank cells
  // that have formatting, therefore we test explicitly for non-blank cell types
  // and only load those cells.
  // 2.4.90 Dimensions p273 of [MS-XLS]
  bool cell_is_readable(const xls::xlsCell *cell) {
    return cell && (
        cell->id == XLS_RECORD_MULRK ||
        cell->id == XLS_RECORD_NUMBER ||
        cell->id == XLS_RECORD_RK ||
        cell->id == XLS_RECORD_LABELSST ||
        cell->id == XLS_RECORD_LABEL ||
        cell->id == XLS_RECORD_FORMULA ||
        cell->id == XLS_RECORD_FORMULA_ALT ||
        cell->id == XLS_RECORD_BOOLERR
    );
  }

};

#endif
