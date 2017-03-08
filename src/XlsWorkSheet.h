#ifndef READXL_XLSWORKSHEET_
#define READXL_XLSWORKSHEET_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "XlsWorkBook.h"
#include "XlsCell.h"
#include "ColSpec.h"

class XlsWorkSheet {
  xls::xlsWorkSheet* pWS_;
  double offset_;
  std::set<int> customDateFormats_;
  std::vector<XlsCell> cells_;
  std::string sheetName_;
  int ncol_, nrow_;
  std::vector<XlsCell>::const_iterator firstRow_, secondRow_;

public:

  XlsWorkSheet(const XlsWorkBook& wb, int sheet_i, int skip) {
    if (sheet_i >= wb.n_sheets()) {
      Rcpp::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                 sheet_i + 1, wb.n_sheets());
    }
    sheetName_ = wb.sheets()[sheet_i];

    pWS_ = xls_getWorkSheet(wb.workbook(), sheet_i);
    if (pWS_ == NULL) {
      Rcpp::stop("Sheet '%s' (position %d): cannot be opened",
                 sheetName_, sheet_i + 1);
    }
    xls_parseWorkSheet(pWS_);
    offset_ = dateOffset(wb.workbook()->is1904);
    customDateFormats_ = wb.customDateFormats();

    loadCells();
    parseGeometry(skip);
  }

  ~XlsWorkSheet() {
    try {
      xls_close_WS(pWS_);
    } catch(...) {}
  }

  int ncol() const {
    return ncol_;
  }

  int nrow() const {
    return nrow_;
  }

  Rcpp::CharacterVector colNames() {
    Rcpp::CharacterVector out(ncol_);
    std::vector<XlsCell>::const_iterator xcell = firstRow_;
    int base = xcell->row();

    while(xcell != cells_.end() && xcell->row() == base) {
      if (xcell->col() >= ncol_) {
        break;
      }
      // revisit after this class gains functions for string conversion
      // https://github.com/tidyverse/readxl/issues/282
      if (xcell->cell()->str == NULL) {
        out[xcell->col()] = NA_STRING;
      } else {
        out[xcell->col()] = Rf_mkCharCE((char*) xcell->cell()->str, CE_UTF8);
      }
      xcell++;
    }
    return out;
  }

  std::vector<ColType> colTypes(std::vector<ColType> types,
                                const StringSet &na,
                                int guess_max = 1000,
                                bool has_col_names = false) {
    std::vector<XlsCell>::const_iterator xcell;
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
      int j = xcell->col();
      if (type_known[j] || j >= ncol_) {
        xcell++;
        continue;
      }
      ColType type = as_ColType(
        xcell->type(na, &pWS_->workbook->xfs, customDateFormats_)
      );
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

    std::vector<XlsCell>::const_iterator xcell;
    xcell = has_col_names ? secondRow_: firstRow_;

    // base is row the data starts on **in the spreadsheet**
    int base = firstRow_->row() + has_col_names;
    int n = (xcell == cells_.end()) ? 0 : nrow_ - base;
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
      int j = xcell->col();
      if ((i + 1) % 1000 == 0) {
        Rcpp::checkUserInterrupt();
      }
      if (types[j] == COL_SKIP || j >= ncol_) {
        xcell++;
        continue;
      }

      CellType type = xcell->type(na, &pWS_->workbook->xfs, customDateFormats_);
      Rcpp::RObject col = cols[j];
      // row to write into
      int row = i - base;
      // Needs to compare to actual cell type to give warnings
      switch(types[j]) {

      case COL_UNKNOWN:
      case COL_BLANK:
      case COL_SKIP:
        break;

      case COL_LOGICAL:
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
        case CELL_LOGICAL:
        case CELL_NUMERIC:
          LOGICAL(col)[row] = xcell->asInteger(na, &pWS_->workbook->xfs,
                  customDateFormats_);
          break;
        case CELL_DATE:
          // print date string here, when/if it's possible to do so
          Rcpp::warning("Expecting logical in [%i, %i] got a date",
                        i + 1, j + 1);
          LOGICAL(col)[row] = NA_LOGICAL;
          break;
        case CELL_TEXT: {
          std::string text_string = xcell->asStdString(na, &pWS_->workbook->xfs,
                                                       customDateFormats_);
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
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_LOGICAL:
          Rcpp::warning("Expecting date in [%i, %i]: got boolean",
                        i + 1, j + 1);
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_DATE:
          REAL(col)[row] = (xcell->cell()->d - offset_) * 86400;
          break;
        case CELL_NUMERIC:
          Rcpp::warning("Coercing numeric to date in [%i, %i]",
                        i + 1, j + 1);
          REAL(col)[row] = (xcell->cell()->d - offset_) * 86400;
          break;
        case CELL_TEXT:
          Rcpp::warning("Expecting date in [%i, %i]: got '%s'",
                        i + 1, j + 1, xcell->cell()->str);
          REAL(col)[row] = NA_REAL;
          break;
        }
        break;

      case COL_NUMERIC:
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_LOGICAL:
          Rcpp::warning("Coercing boolean to numeric in [%i, %i]",
                        i + 1, j + 1);
          REAL(col)[row] = xcell->cell()->d;
          break;
        case CELL_DATE:
          // print date string here, when/if possible
          Rcpp::warning("Expecting numeric in [%i, %i]: got a date",
                        i + 1, j + 1);
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_NUMERIC:
          REAL(col)[row] = xcell->cell()->d;
          break;
        case CELL_TEXT:
        {
          std::string num_string((char*) xcell->cell()->str);
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
        SET_STRING_ELT(col, row,
                       xcell->asCharSxp(na, &pWS_->workbook->xfs, customDateFormats_));
        break;

      case COL_LIST:
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK: {
          SET_VECTOR_ELT(col, row, Rf_ScalarLogical(NA_LOGICAL));
          break;
        }
        case CELL_LOGICAL: {
          SET_VECTOR_ELT(col, row, Rf_ScalarLogical(xcell->cell()->d));
          break;
        }
        case CELL_DATE: {
          Rcpp::RObject cell_val = Rf_ScalarReal((xcell->cell()->d - offset_) * 86400);
          cell_val.attr("class") = Rcpp::CharacterVector::create("POSIXct", "POSIXt");
          cell_val.attr("tzone") = "UTC";
          SET_VECTOR_ELT(col, row, cell_val);
          break;
        }
        case CELL_NUMERIC: {
          SET_VECTOR_ELT(col, row, Rf_ScalarReal(xcell->cell()->d));
          break;
        }
        case CELL_TEXT: {
          std::string stdString((char*) xcell->cell()->str);
          Rcpp::CharacterVector rString = na.contains(stdString) ? NA_STRING : Rf_mkCharCE(stdString.c_str(), CE_UTF8);
          SET_VECTOR_ELT(col, row, rString);
          break;
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

        if (cell->id == 0x27e || cell->id == 0x0BD || cell->id == 0x203 ||
            // cell holds a number:
            //   0x27e -->  638     RK (section 2.4.220) p376 of [MS-XLS]
            //   0x0BD -->  189  MulRk (section 2.4.175) p344
            //   0x203 -->  515 Number (section 2.4.180) p348
            cell->id == 0x06 ||  cell->id == 0x0406 ||
            // cell holds a formula:
            //    0x06 -->   6 Formula (section 2.4.127) p309
            //  0x0406 --> 1030 Formula (Apple Numbers Bug) via libxls
            cell->id == 0x205 ||
            // cell holds either Boolean or error:
            //   0x205 -->  517 BoolErr (section 2.4.24) p216
            cell->id == 0x0FD
            // cell holds a string:
            //   0x0FD -->  253 LabelSst (section 2.4.149) p325
        ) {
          cells_.push_back(cell);
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
  void parseGeometry(int skip) {
    ncol_ = 0;
    nrow_ = 0;

    // empty sheet case
    if (cells_.empty()) {
      return;
    }

    firstRow_ = cells_.end();
    secondRow_ = cells_.end();
    std::vector<XlsCell>::const_iterator it = cells_.begin();

    // advance past skip rows
    while (it != cells_.end() && it->row() < skip) {
      it++;
    }
    // 'skipped past all the data' case
    if (it == cells_.end()) {
      return;
    }

    firstRow_ = it;
    while (it != cells_.end()) {

      if (nrow_ < it->row()) {
        nrow_ = it->row();
      }
      if (ncol_ < it->col()) {
        ncol_ = it->col();
      }

      if (secondRow_ == cells_.end() && it->row() > firstRow_->row()) {
        secondRow_ = it;
      }

      ++it;
    }

    nrow_++;
    ncol_++;
  }

};

#endif
