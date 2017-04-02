#ifndef READXL_XLSWORKSHEET_
#define READXL_XLSWORKSHEET_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "XlsWorkBook.h"
#include "XlsCell.h"
#include "ColSpec.h"
#include "CellLimits.h"

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
  CellLimits empirical_, nominal_, actual_;
  int ncol_, nrow_;
  std::vector<XlsCell>::iterator firstRow_, secondRow_;

public:

  XlsWorkSheet(const XlsWorkBook wb, int sheet_i,
               Rcpp::IntegerVector limits, bool shim):
  wb_(wb)
  {
    if (sheet_i >= wb.n_sheets()) {
      Rcpp::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                 sheet_i + 1, wb.n_sheets());
    }
    sheetName_ = wb.sheets()[sheet_i];

    std::string path = wb_.path();
    pWB_ = xls::xls_open(path.c_str(), "UTF-8");
    pWS_ = xls_getWorkSheet(pWB_, sheet_i);
    if (pWS_ == NULL) {
      Rcpp::stop("Sheet '%s' (position %d): cannot be opened",
                 sheetName_, sheet_i + 1);
    }
    xls_parseWorkSheet(pWS_);
    dateFormats_ = wb.dateFormats();

    loadCells();
    nominal_ = CellLimits(limits);
    setGeometry(shim);
    nrow_ = (actual_.min_row() < 0) ? 0 : actual_.max_row() - actual_.min_row() + 1;
    ncol_ = (actual_.min_col() < 0) ? 0 : actual_.max_col() - actual_.min_col() + 1;
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

  Rcpp::CharacterVector colNames(const StringSet &na) {
    Rcpp::CharacterVector out(ncol_);
    std::vector<XlsCell>::iterator xcell = firstRow_;
    int base = xcell->row();

    while(xcell != cells_.end() && xcell->row() == base) {
      xcell->inferType(na, dateFormats_);
      out[xcell->col() - actual_.min_col()] = xcell->asCharSxp();
      xcell++;
    }
    return out;
  }

  std::vector<ColType> colTypes(std::vector<ColType> types,
                                const StringSet &na,
                                int guess_max = 1000,
                                bool has_col_names = false) {
    std::vector<XlsCell>::iterator xcell;
    xcell = has_col_names ? secondRow_ : firstRow_;

    // no cell data to consult re: types
    if (xcell == cells_.end()) {
      std::fill(types.begin(), types.end(), COL_BLANK);
      return types;
    }

    std::vector<bool> type_known(types.size());
    for (size_t j = 0; j < types.size(); j++) {
      type_known[j] = types[j] != COL_UNKNOWN;
    }

    // base is row the data starts on **in the spreadsheet**
    int base = firstRow_->row() + has_col_names;
    while (xcell != cells_.end() && xcell->row() - base < guess_max) {
      if ((xcell->row() - base + 1) % 1000 == 0) {
        Rcpp::checkUserInterrupt();
      }
      int j = xcell->col() - actual_.min_col();
      if (type_known[j]) {
        xcell++;
        continue;
      }
      xcell->inferType(na, dateFormats_);
      ColType type = as_ColType(xcell->type());
      if (type > types[j]) {
        types[j] = type;
      }
      xcell++;
    }

    return types;
  }

  Rcpp::List readCols(Rcpp::CharacterVector names,
                      const std::vector<ColType>& types,
                      const StringSet &na,
                      bool has_col_names = false) {

    std::vector<XlsCell>::iterator xcell;
    xcell = has_col_names ? secondRow_: firstRow_;

    // base is row the data starts on **in the spreadsheet**
    int base = firstRow_->row() + has_col_names;
    int n = (xcell == cells_.end()) ? 0 : actual_.max_row() - base + 1;
    Rcpp::List cols(ncol_);
    cols.attr("names") = names;
    for (int j = 0; j < ncol_; ++j) {
      cols[j] = makeCol(types[j], n);
    }

    if (n == 0) {
      return cols;
    }

    while (xcell != cells_.end()) {

      int i = xcell->row();
      int j = xcell->col() - actual_.min_col();
      if ((i + 1) % 1000 == 0) {
        Rcpp::checkUserInterrupt();
      }
      if (types[j] == COL_SKIP) {
        xcell++;
        continue;
      }

      xcell->inferType(na, dateFormats_);
      CellType type = xcell->type();
      Rcpp::RObject col = cols[j];
      // row to write into
      int row = i - base;

      // Fit cell of type x into a column of type y
      // Conventions:
      //   * process type in same order as enum, unless reason to do otherwise
      //   * access cell contents only via asWhatever() methods
      switch(types[j]) {

      case COL_UNKNOWN:
      case COL_BLANK:
      case COL_SKIP:
        break;

      case COL_LOGICAL:
        if (type == CELL_DATE) {
          // print date string here, when/if it's possible to do so
          Rcpp::warning("Expecting logical in [%i, %i] got a date",
                        i + 1, j + 1);
        }

        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
        case CELL_LOGICAL:
        case CELL_DATE:
        case CELL_NUMERIC:
          LOGICAL(col)[row] = xcell->asInteger();
          break;
        case CELL_TEXT: {
          std::string text_string = xcell->asStdString();
          bool text_boolean;
          if (logicalFromString(text_string, &text_boolean)) {
            LOGICAL(col)[row] = text_boolean;
          } else {
            Rcpp::warning("Expecting logical in [%i, %i] got '%s'",
                          i + 1, j + 1, text_string);
            LOGICAL(col)[row] = NA_LOGICAL;
          }
        }
          break;
        }
        break;

      case COL_DATE:
        if (type == CELL_LOGICAL) {
          Rcpp::warning("Expecting date in [%i, %i]: got boolean",
                        i + 1, j + 1);
        }
        if (type == CELL_NUMERIC) {
          Rcpp::warning("Coercing numeric to date in [%i, %i]",
                        i + 1, j + 1);
        }
        if (type == CELL_TEXT) {
          Rcpp::warning("Expecting date in [%i, %i]: got '%s'",
                        i + 1, j + 1,
                        xcell->asStdString());
        }
        REAL(col)[row] = xcell->asDate(wb_.is1904());
        break;

      case COL_NUMERIC:
        if (type == CELL_LOGICAL) {
          Rcpp::warning("Coercing boolean to numeric in [%i, %i]",
                        i + 1, j + 1);
        }
        if (type == CELL_DATE) {
          // print date string here, when/if possible
          Rcpp::warning("Expecting numeric in [%i, %i]: got a date",
                        i + 1, j + 1);
        }
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
        case CELL_LOGICAL:
        case CELL_DATE:
        case CELL_NUMERIC:
          REAL(col)[row] = xcell->asDouble();
          break;
        case CELL_TEXT:
        {
          std::string num_string = xcell->asStdString();
          double num_num;
          bool success = doubleFromString(num_string, num_num);
          if (success) {
            Rcpp::warning("Coercing text to numeric in [%i, %i]: '%s'",
                          i + 1, j + 1, num_string);
            REAL(col)[row] = num_num;
          } else {
            Rcpp::warning("Expecting numeric in [%i, %i]: got '%s'",
                          i + 1, j + 1, num_string);
            REAL(col)[row] = NA_REAL;
          }
        }
          break;
        }
        break;

      case COL_TEXT:
        // not issuing warnings for NAs or coercion, because "text" is the
        // fallback column type and there are too many warnings to be helpful
        SET_STRING_ELT(col, row, xcell->asCharSxp());
        break;

      case COL_LIST:
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
          SET_VECTOR_ELT(col, row, Rf_ScalarLogical(NA_LOGICAL));
          break;
        case CELL_LOGICAL:
          SET_VECTOR_ELT(col, row, Rf_ScalarLogical(xcell->asInteger()));
          break;
        case CELL_DATE: {
          Rcpp::RObject cell_val = Rf_ScalarReal(xcell->asDate(wb_.is1904()));
          cell_val.attr("class") = Rcpp::CharacterVector::create("POSIXct", "POSIXt");
          cell_val.attr("tzone") = "UTC";
          SET_VECTOR_ELT(col, row, cell_val);
          break;
        }
        case CELL_NUMERIC:
          SET_VECTOR_ELT(col, row, Rf_ScalarReal(xcell->asDouble()));
          break;
        case CELL_TEXT: {
          Rcpp::CharacterVector rStringVector = Rcpp::CharacterVector(1, NA_STRING);
          SET_STRING_ELT(rStringVector, 0, xcell->asCharSxp());
          SET_VECTOR_ELT(col, row, rStringVector);
        }
      }
      }
      xcell++;
    }

    return removeSkippedColumns(cols, names, types);
  }

private:

  void loadCells() {
    int nominal_ncol = pWS_->rows.lastcol;
    int nominal_nrow = pWS_->rows.lastrow;

    for (xls::WORD i = 0; i <= nominal_nrow; ++i) {
      for (xls::WORD j = 0; j <= nominal_ncol; ++j) {

        xls::xlsCell *cell = xls_cell(pWS_, i, j);

        if (!cell) {
          continue;
        }

        // Dimensions reported by xls itself include empty cells that have
        // formatting, therefore we test explicitly for non-blank cell types
        // and only load those cells.
        // 2.4.90 Dimensions p273 of [MS-XLS]
        if (cell->id == XLS_RECORD_MULRK || cell->id == XLS_RECORD_NUMBER ||
            cell->id == XLS_RECORD_RK ||
            cell->id == XLS_RECORD_LABELSST || cell->id == XLS_RECORD_LABEL ||
            cell->id == XLS_RECORD_FORMULA ||  cell->id == XLS_RECORD_FORMULA_ALT ||
            cell->id == XLS_RECORD_BOOLERR
        ) {
          cells_.push_back(cell);
          empirical_.update(cells_.back());
        }
      }
    }
  }

  // Compute sheet extent (= lower right corner) directly from loaded cells.
  //   recorded in nrow_ and ncol_
  // Return early if there is no data. Otherwise ...
  // Position iterators at two landmarks for reading:
  //   firstRow_ = first cell for which declared row >= skip
  //   secondRow_ = first cell for which declared row > that of firstRow_
  //   fallback to cells_.end() if the above not possible
  // Assumes loaded cells are arranged s.t. row is non-decreasing
  void setGeometry(bool shim) {

    // empty sheet or 'read no data' case (convention is min_row = -2)
    if (cells_.empty() || nominal_.min_row() < -1) {
      return;
    }

    // remove cells that lie outside the nominal rectangle
    // there should be some way to to this with erase-remove,
    // but damn if I can figure that out
    for (std::vector<XlsCell>::iterator it = cells_.begin();
         it != cells_.end(); ) {
      if (falls_outside(nominal_, *it)) {
        it = cells_.erase(it);
      } else {
        ++it;
      }
    }

    if (cells_.empty()) {
      return;
    }

    std::vector<XlsCell>::iterator it = cells_.begin();

    if (shim) {
      bool shim_up = nominal_.min_row() >= 0 && empirical_.min_row() > nominal_.min_row();
      bool shim_left = nominal_.min_col() >= 0 && empirical_.min_col() > nominal_.min_col();
      if (shim_up || shim_left) {
        int ul_row = nominal_.min_row() >= 0 ? nominal_.min_row() : empirical_.min_row();
        int ul_col = nominal_.min_col() >= 0 ? nominal_.min_col() : empirical_.min_col();
        XlsCell ul_shim(std::make_pair(ul_row, ul_col));
        it = cells_.insert(it, ul_shim);
      }

      bool shim_down = nominal_.max_row() >= 0 && empirical_.max_row() < nominal_.max_row();
      bool shim_right = nominal_.max_col() >= 0 && empirical_.max_col() < nominal_.max_col();
      if (shim_down || shim_right) {
        int lr_row = nominal_.max_row() >= 0 ? nominal_.max_row() : empirical_.max_row();
        int lr_col = nominal_.max_col() >= 0 ? nominal_.max_col() : empirical_.max_col();
        XlsCell lr_shim(std::make_pair(lr_row, lr_col));
        cells_.push_back(lr_shim);
        it = cells_.begin();
      }
    }

    firstRow_ = it;
    secondRow_ = cells_.end();
    while (it != cells_.end()) {
      actual_.update(*it);
      if (secondRow_ == cells_.end() && it->row() > firstRow_->row()) {
        secondRow_ = it;
      }
      ++it;
    }
  }

  bool falls_outside(const CellLimits limits, const XlsCell cell) {
    return !limits.contains(cell);
  }

};

#endif
