#ifndef EXELL_XLSWORKSHEET_
#define EXELL_XLSWORKSHEET_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "CellType.h"

class XlsWorkSheet {
  xls::xlsWorkSheet* pWS_;
  int nrow_, ncol_;

public:

  XlsWorkSheet(const XlsWorkBook& wb, int i) {
    pWS_ = xls_getWorkSheet(wb.workbook(), 0);
    if (pWS_ == NULL)
      Rcpp::stop("Failed open sheet");

    xls_parseWorkSheet(pWS_);

    nrow_ = pWS_->rows.lastrow + 1;
    ncol_ = pWS_->rows.lastcol + 1;
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

};

#endif
