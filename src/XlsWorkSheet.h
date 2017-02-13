#ifndef READXL_XLSWORKSHEET_
#define READXL_XLSWORKSHEET_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "XlsWorkBook.h"
#include "CellType.h"
#include "utils.h"

class XlsWorkSheet {
  xls::xlsWorkSheet* pWS_;
  int nrow_, ncol_;
  double offset_;
  std::set<int> customDateFormats_;

public:

  XlsWorkSheet(const XlsWorkBook& wb, int i) {
    offset_ = dateOffset(wb.workbook()->is1904);

    if (i >= wb.nSheets()) {
      Rcpp::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                 i + 1, wb.nSheets());
    }

    pWS_ = xls_getWorkSheet(wb.workbook(), i);
    if (pWS_ == NULL)
      Rcpp::stop("Failed open sheet");
    xls_parseWorkSheet(pWS_);

    nrow_ = pWS_->rows.lastrow + 1;
    ncol_ = pWS_->rows.lastcol; // excel always pads with an empty column

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

  Rcpp::CharacterVector colNames(int nskip = 0) {
    Rcpp::CharacterVector out(ncol_);

    if (nskip > nrow_)
      return out;

    xls::st_row::st_row_data row = pWS_->rows.row[nskip];
    for (int j = 0; j < ncol_; ++j) {
      xls::st_cell::st_cell_data cell = row.cells.cell[j];
      if (cell.str == NULL) {
        out[j] = NA_STRING;
      } else {
        out[j] = Rf_mkCharCE((char*) cell.str, CE_UTF8);
      }
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
            Rcpp::as<Rcpp::List>(col)[i] = Rcpp::LogicalVector(1, NA_LOGICAL);
            break;
          }
          case CELL_NUMERIC: {
            Rcpp::as<Rcpp::List>(col)[i] = Rcpp::NumericVector(1, cell.d);
            break;
          }
          case CELL_DATE: {
            Rcpp::RObject cell_val = Rcpp::NumericVector(1, (cell.d - offset_) * 86400);
            cell_val.attr("class") = Rcpp::CharacterVector::create("POSIXct", "POSIXt");
            cell_val.attr("tzone") = "UTC";
            Rcpp::as<Rcpp::List>(col)[i] = cell_val;
            break;
          }
          case CELL_TEXT: {
            Rcpp::CharacterVector rStringVector = Rcpp::CharacterVector(1, NA_STRING);
            std::string stdString((char*) cell.str);
            Rcpp::RObject rString = na.contains(stdString) ? NA_STRING : Rf_mkCharCE(stdString.c_str(), CE_UTF8);
            SET_STRING_ELT(rStringVector, 0, rString);
            Rcpp::as<Rcpp::List>(col)[i] = rStringVector;
            break;
          }
          }
        }
      }
    }

    return removeSkippedColumns(cols, names, types);
  }
};

#endif
