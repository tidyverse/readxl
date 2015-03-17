#ifndef EXELL_XLSWORKSHEET_
#define EXELL_XLSWORKSHEET_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "CellType.h"

class XlsWorkSheet {
  xls::xlsWorkSheet* pWS_;
  int nrow_, ncol_;
  double offset_;

public:

  XlsWorkSheet(const XlsWorkBook& wb, int i) {
    if (wb.workbook()->is1904) {
      offset_ = 2082844800; // as.numeric(as.POSIXct(as.Date("1904-01-01")))
    } else {
      offset_ = 2208988800; // as.numeric(as.POSIXct(as.Date("1900-01-01")))
    }

    pWS_ = xls_getWorkSheet(wb.workbook(), 0);
    if (pWS_ == NULL)
      Rcpp::stop("Failed open sheet");
    xls_parseWorkSheet(pWS_);

    nrow_ = pWS_->rows.lastrow + 1;
    ncol_ = pWS_->rows.lastcol; // excel always pads with an empty column
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

  CharacterVector colNames(int nskip = 0) {
    CharacterVector out(ncol_);

    if (nskip > nrow_)
      return out;

    xls::st_row::st_row_data row = pWS_->rows.row[nskip];
    for (int j = 0; j < ncol_; ++j) {
      xls::st_cell::st_cell_data cell = row.cells.cell[j];
      if (cell.str == NULL) {
        out[j] = NA_STRING;
      } else {
        out[j] = std::string((char*) cell.str);
      }
    }

    return out;
  }

  std::vector<CellType> colTypes(int nskip = 0, int n_max = 100) {
    std::vector<CellType> types(ncol_);

    for (int i = nskip; i < nrow_ && i < n_max; ++i) {
      if ((i + 1) % 10000 == 0)
        checkUserInterrupt();

      xls::st_row::st_row_data row = pWS_->rows.row[i];

      for (int j = 0; j < ncol_; ++j) {
        CellType type = cellType(row.cells.cell[j]);

        // Excel is simple enough we can enforce a strict ordering
        if (type > types[j]) {
          types[j] = type;
        }
      }
    }

    return types;
  }

  Rcpp::List readCols(CharacterVector names, std::vector<CellType> types,
                      int nskip = 0) {
    if (names.size() != ncol_ || types.size() != ncol_)
      Rcpp::stop("Need one name and type for each column");

    Rcpp::List out(ncol_);

    // Initialise columns
    for (int j = 0; j < ncol_; ++j) {
      switch(types[j]) {
      case CELL_BLANK:
        out[j] = Rcpp::LogicalVector(nrow_);
        break;
      case CELL_DATE: {
          RObject col = Rcpp::NumericVector(nrow_);
          col.attr("class") = CharacterVector::create("POSIXct", "POSIXt");
          out[j] = col;
        }
        break;
      case CELL_NUMERIC:
        out[j] = Rcpp::NumericVector(nrow_);
        break;
      case CELL_TEXT:
        out[j] = Rcpp::CharacterVector(nrow_);
        break;
      }
    }

    // Fill with data
    for (int i = nskip; i < nrow_; ++i) {
      xls::st_row::st_row_data row = pWS_->rows.row[i];

      for (int j = 0; j < ncol_; ++j) {
        xls::st_cell::st_cell_data cell = row.cells.cell[j];
        RObject col = out[j];

        // Needs to compare to actual cell type to give warnings
        switch(types[j]) {
        case CELL_BLANK:
          break;
        case CELL_NUMERIC:
          REAL(col)[i] = isBlank(cell) ? NA_REAL : cell.d;
          break;
        case CELL_DATE:
          REAL(col)[i] = isBlank(cell) ? NA_REAL : cell.d - offset_;
          break;
        case CELL_TEXT:
          if (isBlank(cell)) {
            SET_STRING_ELT(col, i, NA_STRING);
          } else {
            RObject string = Rf_mkCharCE((char*) cell.str, CE_UTF8);
            SET_STRING_ELT(col, i, string);
          }
          break;
        }
      }
    }

    out.attr("class") = CharacterVector::create("tbl_df", "tbl", "data.frame");
    out.attr("row.names") = IntegerVector::create(NA_INTEGER, -nrow_);
    out.attr("names") = names;

    return out;
  }
};

#endif
