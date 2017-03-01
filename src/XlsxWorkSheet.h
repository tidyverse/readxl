#ifndef READXL_XLSXWORKSHEET_
#define READXL_XLSXWORKSHEET_

#include <Rcpp.h>
#include "rapidxml.h"
#include "XlsxWorkBook.h"
#include "XlsxCell.h"
#include "ColSpec.h"

// Key reference for understanding the structure of the XML is
// ECMA-376 (http://www.ecma-international.org/publications/standards/Ecma-376.htm)
// Section and page numbers below refer to the 4th edition
// 18.3.1.73  row         (Row)        [p1677]
// 18.3.1.4   c           (Cell)       [p1598]
// 18.3.1.96  v           (Cell Value) [p1709]
// 18.18.11   ST_CellType (Cell Type)  [p2443]

class XlsxWorkSheet {
  XlsxWorkBook wb_;
  std::string sheet_;
  rapidxml::xml_document<> sheetXml_;
  rapidxml::xml_node<>* sheetData_;
  std::vector<XlsxCell> cells_;
  std::string sheetName_;
  int ncol_, nrow_;
  std::vector<XlsxCell>::const_iterator firstRow_, secondRow_;

public:

  XlsxWorkSheet(const XlsxWorkBook wb, int sheet_i, int skip):
  wb_(wb)
  {
    rapidxml::xml_node<>* rootNode;

    if (sheet_i >= wb.n_sheets()) {
      Rcpp::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                 sheet_i + 1,  wb.n_sheets());
    }
    sheetName_ = wb.sheets()[sheet_i];
    std::string sheetPath = wb.sheetPath(sheet_i);
    sheet_ = zip_buffer(wb.path(), sheetPath);
    sheetXml_.parse<0>(&sheet_[0]);

    rootNode = sheetXml_.first_node("worksheet");
    if (rootNode == NULL) {
      Rcpp::stop("Sheet '%s' (position %d): Invalid sheet xml (no <worksheet>)",
                 sheetName_, sheet_i + 1);
    }

    sheetData_ = rootNode->first_node("sheetData");
    if (sheetData_ == NULL) {
      Rcpp::stop("Sheet '%s' (position %d): Invalid sheet xml (no <sheetData>)",
                 sheetName_, sheet_i + 1);
    }

    loadCells();
    parseGeometry(skip);
  }

  int ncol() const {
    return ncol_;
  }

  int nrow() const {
    return nrow_;
  }

  std::string sheetName() const {
    return sheetName_;
  }

  Rcpp::CharacterVector colNames() {
    Rcpp::CharacterVector out(ncol_);
    std::vector<XlsxCell>::const_iterator xcell = firstRow_;
    int base = xcell->row();

    while(xcell != cells_.end() && xcell->row() == base) {
      if (xcell->col() >= ncol_) {
        break;
      }
      out[xcell->col()] = xcell->asCharSxp("", wb_.stringTable());
      xcell++;
    }
    return out;
  }

  std::vector<ColType> colTypes(const StringSet& na,
                                int guess_max = 1000,
                                bool has_col_names = false) {
    std::vector<ColType> types(ncol_);

    std::vector<XlsxCell>::const_iterator xcell;
    xcell = has_col_names ? secondRow_ : firstRow_;

    // no cell data to consult re: types
    if (xcell == cells_.end()) {
      std::fill(types.begin(), types.end(), COL_BLANK);
      return types;
    }

    // base is row the data starts on **in the spreadsheet**
    int base = firstRow_->row() + has_col_names;
    while (xcell != cells_.end() && xcell->row() - base < guess_max) {
      if ((xcell->row() - base + 1) % 1000 == 0) {
        Rcpp::checkUserInterrupt();
      }
      if (xcell->col() < ncol_) {
        ColType type = as_ColType(
          xcell->type(na, wb_.stringTable(), wb_.dateStyles())
        );
        if (type > types[xcell->col()]) {
          types[xcell->col()] = type;
        }
      }
      xcell++;
    }

    return types;
  }

  Rcpp::List readCols(Rcpp::CharacterVector names,
                      const std::vector<ColType>& types,
                      const StringSet& na,
                      bool has_col_names = false) {

    std::vector<XlsxCell>::const_iterator xcell;
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

      CellType type = xcell->type(na, wb_.stringTable(), wb_.dateStyles());
      Rcpp::RObject col = cols[j];
      // row to write into
      int row = i - base;
      // Needs to compare to actual cell type to give warnings
      switch(types[j]) {

      case COL_BLANK:
      case COL_SKIP:
        break;

      case COL_LOGICAL:
        switch(type) {
        case CELL_BLANK:
          LOGICAL(col)[row] = NA_LOGICAL;
          break;
        case CELL_DATE:
          // print date string here, when/if it's possible to do so
          Rcpp::warning("Expecting logical in [%i, %i]: got a date",
                        i + 1, j + 1);
          LOGICAL(col)[row] = NA_LOGICAL;
          break;
        case CELL_LOGICAL:
        case CELL_NUMERIC:
          LOGICAL(col)[row] = xcell->asInteger(na) ? TRUE : FALSE;
          break;
        case CELL_TEXT:
        {
          std::string text_string = xcell->asStdString(na, wb_.stringTable(),
                                                       wb_.dateStyles());
          bool text_boolean;
          if (logicalFromString(text_string, &text_boolean)) {
            LOGICAL(col)[row] = text_boolean ? TRUE : FALSE;
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
        case CELL_BLANK:
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_LOGICAL:
          Rcpp::warning("Expecting date in [%i, %i]: got boolean",
                        i + 1, j + 1);
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_DATE:
          REAL(col)[row] = xcell->asDate(na, wb_.offset());
          break;
        case CELL_NUMERIC:
          Rcpp::warning("Expecting date in [%i, %i]: got %f",
                        i + 1, j + 1, xcell->asDouble(na));
          REAL(col)[row] = NA_REAL;
        case CELL_TEXT:
          Rcpp::warning("Expecting date in [%i, %i]: got '%s'",
                        i + 1, j + 1, xcell->asStdString(na, wb_.stringTable(),
                                                         wb_.dateStyles()));
          REAL(col)[row] = NA_REAL;
          break;
        }
        break;

      case COL_NUMERIC:
        switch(type) {
        case CELL_BLANK:
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_LOGICAL:
          Rcpp::warning("Coercing boolean to numeric in [%i, %i]",
                        i + 1, j + 1);
          REAL(col)[row] = xcell->asDouble(na);
          break;
        case CELL_DATE:
          // print date string here, when it's easier to do so
          Rcpp::warning("Expecting numeric in [%i, %i]: got a date",
                        i + 1, j + 1);
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_NUMERIC:
          REAL(col)[row] = xcell->asDouble(na);
          break;
        case CELL_TEXT:
          Rcpp::warning("Expecting numeric in [%i, %i]: got '%s'",
                        i + 1, j + 1, xcell->asStdString(na, wb_.stringTable(),
                                                         wb_.dateStyles()));
          REAL(col)[row] = NA_REAL;
        }
        break;

      case COL_TEXT:
        // not issuing warnings for NAs or coercion, because "text" is the
        // fallback column type and there are too many warnings to be helpful
        switch(type) {
        case CELL_BLANK:
          SET_STRING_ELT(col, row, NA_STRING);
          break;
        case CELL_LOGICAL:
        {
          int true_or_false = xcell->asInteger(na);
          if (true_or_false) {
            SET_STRING_ELT(col, row, Rf_mkChar("TRUE"));
          } else {
            SET_STRING_ELT(col, row, Rf_mkChar("FALSE"));
          }
        }
          break;
        case CELL_DATE:
          // would be nice: use date string
          SET_STRING_ELT(col, row, xcell->asCharSxp(na, wb_.stringTable()));
          break;
        case CELL_NUMERIC:
        case CELL_TEXT:
        {
          SET_STRING_ELT(col, row, xcell->asCharSxp(na, wb_.stringTable()));
        }
        }
        break;

      case COL_LIST:
        switch(type) {
        case CELL_BLANK: {
          SET_VECTOR_ELT(col, row, Rf_ScalarLogical(NA_LOGICAL));
          break;
        }
        case CELL_LOGICAL: {
          SET_VECTOR_ELT(col, row, Rf_ScalarLogical(xcell->asInteger(na)));
          break;
        }
        case CELL_DATE: {
          Rcpp::RObject cell_val = Rf_ScalarReal(xcell->asDate(na, wb_.offset()));
          cell_val.attr("class") = Rcpp::CharacterVector::create("POSIXct", "POSIXt");
          cell_val.attr("tzone") = "UTC";
          SET_VECTOR_ELT(col, row, cell_val);
          break;
        }
        case CELL_NUMERIC: {
          SET_VECTOR_ELT(col, row, Rf_ScalarReal(xcell->asDouble(na)));
          break;
        }
        case CELL_TEXT: {
          Rcpp::CharacterVector rStringVector = Rcpp::CharacterVector(1, NA_STRING);
          SET_STRING_ELT(rStringVector, 0, xcell->asCharSxp(na, wb_.stringTable()));
          SET_VECTOR_ELT(col, row, rStringVector);
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
    rapidxml::xml_node<>* row = sheetData_->first_node("row");
    if (row == NULL) {
      return;
    }

    int i = 0;
    for (; row; row = row->next_sibling("row")) {
      int j = 0;
      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {
        rapidxml::xml_node<>* first_child = cell->first_node(0);
        // only load cells that have >= 1 child nodes
        // we require cell to have content, not just, e.g., a style
        if (first_child != NULL) {
          XlsxCell xcell(cell, i, j);
          cells_.push_back(xcell);
        }
        j++;
      }
      i++;
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
    nrow_ = 0;
    ncol_ = 0;

    // empty sheet case
    if (cells_.empty()) {
      return;
    }

    firstRow_ = cells_.end();
    secondRow_ = cells_.end();
    std::vector<XlsxCell>::const_iterator it = cells_.begin();

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
