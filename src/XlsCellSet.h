#pragma once

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
#include "cpp11/R.hpp"
#include "cpp11/sexp.hpp"
#include "cpp11/strings.hpp"

class XlsCellSet {
  // xls specifics
  xls::xlsWorkBook* pWB_;
  xls::xlsWorkSheet* pWS_;

  // common to xls[x]
  std::string sheetName_;
  CellLimits nominal_, actual_;
  int ncol_, nrow_;

public:

  std::vector<XlsCell> cells_;

  XlsCellSet(const XlsWorkBook wb, int sheet_i,
             cpp11::integers limits, bool shim, Spinner spinner_)
    : nominal_(limits)
  {
    if (sheet_i >= wb.n_sheets()) {
      cpp11::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                  sheet_i + 1, wb.n_sheets());
    }
    sheetName_ = wb.sheets()[sheet_i];

    xls::xls_error_t error = xls::LIBXLS_OK;
    spinner_.spin();
    pWB_ = xls_open_file(wb.path().c_str(), "UTF-8", &error);
    if (!pWB_) {
      Rf_errorcall(
        R_NilValue,
        "\n  filepath: %s\n  libxls error: %s",
        wb.path().c_str(),
        xls::xls_getError(error)
      );
    }
    spinner_.spin();

    pWS_ = xls::xls_getWorkSheet(pWB_, sheet_i);
    if (!pWS_) {
      cpp11::stop("Sheet '%s' (position %d): cannot be opened",
                  sheetName_.c_str(), sheet_i + 1);
    }
    error = xls::xls_parseWorkSheet(pWS_);
    if (error != xls::LIBXLS_OK) {
      Rf_errorcall(
        R_NilValue,
        "\n  filepath: %s\n  sheet: %s\n  libxls error: %s",
        wb.path().c_str(),
        sheetName_.c_str(),
        xls::xls_getError(error)
      );
    }
    spinner_.spin();

    // shim = TRUE when user specifies geometry via `range`
    // shim = FALSE when user specifies no geometry or uses `skip` and `n_max`
    // nominal_ holds user's geometry request, where -1 means "unspecified"
    loadCells(shim, spinner_);
    // nominal_ may have been shifted (case of implicit skipping and n_max)
    // actual_ reports populated cells inside the nominal_ rectangle

    // When shim = FALSE, we shrink-wrap the data that falls inside
    // the nominal_ rectangle.
    //
    // When shim = TRUE, we may need to insert dummy cells to fill out
    // the nominal_rectangle.
    // actual_ is updated to reflect the insertions.
    if (shim) insertShims(cells_, nominal_, actual_);

    nrow_ = (actual_.minRow() < 0) ? 0 : actual_.maxRow() - actual_.minRow() + 1;
    ncol_ = (actual_.minCol() < 0) ? 0 : actual_.maxCol() - actual_.minCol() + 1;
  }

  ~XlsCellSet() {
    try {
      xls::xls_close_WS(pWS_);
      xls::xls_close_WB(pWB_);
    } catch(...) {}
  }

  int ncol() const { return ncol_; }
  int nrow() const { return nrow_; }
  std::string sheetName() const {return sheetName_; }
  int startCol() const { return actual_.minCol(); }
  int lastRow() const { return actual_.maxRow(); }

private:

  void loadCells(const bool shim, Spinner spinner_) {
    // by convention, min_row = -2 means 'read no data'
    if (nominal_.minRow() < -1) {
      return;
    }

    // count is for spinner and checking for interrupt
    int count = 0;

    int nominal_ncol = pWS_->rows.lastcol;
    int nominal_nrow = pWS_->rows.lastrow;

    xls::xlsCell* cell;
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

  // Dimensions reported by xls and cells contained in xls include blank cells
  // that have formatting, therefore we test explicitly for non-blank cell types
  // and only load those cells.
  // 2.4.90 Dimensions p273 of [MS-XLS]
  bool cell_is_readable(const xls::xlsCell* cell) {
    return cell && (
        cell->id == XLS_RECORD_MULRK ||
        cell->id == XLS_RECORD_RSTRING ||
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
