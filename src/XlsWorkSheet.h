#ifndef READXL_XLSWORKSHEET_
#define READXL_XLSWORKSHEET_

#include <Rcpp.h>
#include <libxls/xls.h>
#include <libxls/xlstypes.h>
#include "XlsCell.h"
#include "XlsWorkBook.h"
#include "CellType.h"
#include "utils.h"

class XlsWorkSheet {
  xls::xlsWorkSheet* pWS_;
  std::vector<XlsCell> cells_;
  std::string sheetName_;
  int ncol_, nrow_;
  std::vector<XlsCell>::const_iterator firstRow_, secondRow_;
  double offset_;
  std::set<int> customDateFormats_;

public:

  XlsWorkSheet(const XlsWorkBook& wb, int sheet_i, int nskip) {
    offset_ = dateOffset(wb.workbook()->is1904);

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
    loadCells();
    parseGeometry(nskip);
    // Rcpp::Rcout << "back from parseGeometry()\n";
    //Rcpp::Rcout << "nrow_ = " << nrow_ << ", ncol_ = " << ncol_ << "\n";
    //Rcpp::Rcout << "first_row_ = " << first_row_ <<
    //  ", second_row_ = " << second_row_ << "\n";

    customDateFormats_ = wb.customDateFormats();
  }

  ~XlsWorkSheet() {
    try {
      xls_close_WS(pWS_);
    } catch(...) {}
  }

  int nrow() const {
    return nrow_;
  }

  int ncol() const {
    return ncol_;
  }

  Rcpp::CharacterVector colNames() {
    Rcpp::CharacterVector out(ncol_);
    std::vector<XlsCell>::const_iterator xcell = firstRow_;
    int base = xcell->row();

    while(xcell != cells_.end() && xcell->row() == base) {
      if (xcell->col() >= ncol_) {
        break;
      }
      if (xcell->cell()->str == NULL) {
        out[xcell->col()] = NA_STRING;
      } else {
        out[xcell->col()] = Rf_mkCharCE((char*) xcell->cell()->str, CE_UTF8);
      }
      xcell++;
    }
    return out;
  }

  std::vector<ColType> colTypes(const StringSet &na, int nskip = 0,
                                 int guess_max = 1000) {
    std::vector<ColType> types(ncol_);

    for (int i = nskip; i < nrow_ && i < nskip + guess_max; ++i) {
      if ((i + 1) % 10000 == 0)
        Rcpp::checkUserInterrupt();

      xls::st_row::st_row_data row = pWS_->rows.row[i];

      for (int j = 0; j < ncol_; ++j) {
        ColType type = as_ColType(cellType(row.cells.cell[j], &pWS_->workbook->xfs, customDateFormats_, na));

        // Excel is simple enough we can enforce a strict ordering
        if (type > types[j]) {
          types[j] = type;
        }
      }
    }

    return types;
  }

  Rcpp::List readCols(Rcpp::CharacterVector names, std::vector<ColType> types,
                      const StringSet &na, int nskip = 0) {
    if ((int) names.size() != ncol_ || (int) types.size() != ncol_){
      Rcpp::stop("Received %d names and %d types, but worksheet contains %d columns.",
                 names.size(), types.size(),  ncol_);
    }

    Rcpp::List cols(ncol_);

    // Initialise columns
    int n = nrow_ - nskip;
    for (int j = 0; j < ncol_; ++j) {
      cols[j] = makeCol(types[j], n);
    }

    // Fill with data
    for (int i = 0; i < n; ++i) {
      xls::st_row::st_row_data row = pWS_->rows.row[i + nskip];

      for (int j = 0; j < ncol_; ++j) {
        xls::st_cell::st_cell_data cell = row.cells.cell[j];
        Rcpp::RObject col = cols[j];

        CellType type = cellType(cell, &pWS_->workbook->xfs, customDateFormats_, na);

        // Needs to compare to actual cell type to give warnings
        switch(types[j]) {
        case COL_BLANK:
        case COL_SKIP:
          break;
        case COL_NUMERIC:
          switch(type) {
          case CELL_BLANK:
            REAL(col)[i] = NA_REAL;
            break;
          case CELL_NUMERIC:
          case CELL_DATE:
            REAL(col)[i] = cell.d;
            break;
          case COL_TEXT:
            Rcpp::warning("Expecting numeric in [%i, %i] got `%s`",
              i + 1, j + 1, (char*) cell.str);
            REAL(col)[i] = NA_REAL;
          }
          break;
        case COL_DATE:
          switch(type) {
          case CELL_BLANK:
            REAL(col)[i] = NA_REAL;
            break;
          case CELL_NUMERIC:
            Rcpp::warning("Expecting date in [%i, %i] got %d",
              i + 1, j + 1, cell.d);
            REAL(col)[i] = NA_REAL;
            break;
          case CELL_DATE:
            REAL(col)[i] = (cell.d - offset_) * 86400;
            break;
          case CELL_TEXT:
            Rcpp::warning("Expecting date in [%i, %i] got '%s'",
              i + 1, j + 1, cell.str);
            REAL(col)[i] = NA_REAL;
            break;
          }
          break;
        case COL_TEXT:
          if (type == CELL_BLANK) {
            SET_STRING_ELT(col, i, NA_STRING);
          } else {
            std::string stdString((char*) cell.str);
            Rcpp::RObject rString = na.contains(stdString) ? NA_STRING : Rf_mkCharCE(stdString.c_str(), CE_UTF8);
            SET_STRING_ELT(col, i, rString);
          }
          break;
        case COL_LIST:
          switch(type) {
          case CELL_BLANK: {
            SET_VECTOR_ELT(col, i, Rf_ScalarLogical(NA_LOGICAL));
            break;
          }
          case CELL_NUMERIC: {
            SET_VECTOR_ELT(col, i, Rf_ScalarReal(cell.d));
            break;
          }
          case CELL_DATE: {
            Rcpp::RObject cell_val = Rf_ScalarReal((cell.d - offset_) * 86400);
            cell_val.attr("class") = Rcpp::CharacterVector::create("POSIXct", "POSIXt");
            cell_val.attr("tzone") = "UTC";
            SET_VECTOR_ELT(col, i, cell_val);
            break;
          }
          case CELL_TEXT: {
            std::string stdString((char*) cell.str);
            Rcpp::CharacterVector rString = na.contains(stdString) ? NA_STRING : Rf_mkCharCE(stdString.c_str(), CE_UTF8);
            SET_VECTOR_ELT(col, i, rString);
            break;
          }
          }
        }
      }
    }

    return removeSkippedColumns(cols, names, types);
  }

private:

  void loadCells() {
    int nominal_ncol = pWS_->rows.lastcol;
    int nominal_nrow = pWS_->rows.lastrow;

    for (xls::WORD i = 0; i <= nominal_nrow; ++i) {
      for (xls::WORD j = 0; j <= nominal_ncol; ++j) {
        //Rcpp::Rcout << "row = " << i + 1 << ", col = " << j + 1 << "\n";
        xls::xlsCell *cell = xls_cell(pWS_, i, j);

        if (!cell) {
          //Rcpp::Rcout << "no cell found!\n";
          continue;
        }

        if (cell->id == 0x27e || cell->id == 0x0BD || cell->id == 0x203 ||
            // cell holds a number:
            //   0x27e --> 638     RK (section 2.4.220) p376 of [MS-XLS]
            //   0x0BD --> 189  MulRk (section 2.4.175) p344
            //   0x203 --> 515 Number (section 2.4.180) p348
            cell->id == 0x06 ||
            // cell holds a formula:
            //    0x06 -->   6 Formula (section 2.4.127) p309
            cell->id == 0x205 ||
            // cell holds either Boolean or error:
            //   0x205 --> 517 BoolErr (section 2.4.24) p216
            cell->id == 0x0FD
            // cell holds a string:
            //   0x0FD --> 253 LabelSst (section 2.4.149) p325
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
  //   firstRow_ = first cell for which declared row >= nskip
  //   secondRow_ = first cell for which declared row > that of firstRow_
  //   fallback to cells_.end() if the above not possible
  // Assumes loaded cells are arranged s.t. row is non-decreasing
  void parseGeometry(int nskip) {
    ncol_ = 0;
    nrow_ = 0;

    // empty sheet case
    if (cells_.size() == 0) {
      return;
    }

    // Rcpp::Rcout << "nskip = " << nskip << "\n";

    firstRow_ = cells_.end();
    secondRow_ = cells_.end();
    std::vector<XlsCell>::const_iterator it = cells_.begin();

    // advance past nskip rows
    while (it != cells_.end() && it->row() < nskip) {
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

  // Rcpp::CharacterVector out(1);
  // if (cell->str == NULL) {
  //   out[0] = NA_STRING;
  // } else {
  //   out[0] = Rf_mkCharCE((char*) cell->str, CE_UTF8);
  // }
  // Rcpp::Rcout << "contents of lower right corner cell = " << out[0] << "\n";

};

#endif
