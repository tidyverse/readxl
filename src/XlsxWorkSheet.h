#ifndef READXL_XLSXWORKSHEET_
#define READXL_XLSXWORKSHEET_

#include <Rcpp.h>
#include "rapidxml.h"
#include "XlsxWorkBook.h"
#include "XlsxCell.h"

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

  XlsxWorkSheet(XlsxWorkBook wb, int sheet_i, int nskip):
  wb_(wb)
  {
    rapidxml::xml_node<>* rootNode;

    if (sheet_i > wb.n_sheets()) {
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
    parseGeometry(nskip);
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

  std::vector<CellType> colTypes(const StringSet& na,
                                 int guess_max = 1000,
                                 bool has_col_names = false) {
    std::vector<CellType> types;
    types.resize(ncol_);

    std::vector<XlsxCell>::const_iterator it;
    it = has_col_names ? secondRow_: firstRow_;

    // no cell data to consult re: types
    if (it == cells_.end()) {
      for (size_t i = 0; i < types.size(); i++) {
        types[i] = CELL_BLANK;
      }
      return types;
    }

    int base = firstRow_->row() + has_col_names;
    // we have consulted i rows re: determining col types
    int i;
    // account for any empty rows between column headers and data start
    i = it->row() - base;
    // m is the max row number seen so far
    int m = it->row();

    while (i < guess_max && it != cells_.end()) {
      XlsxCell xcell = *it;
      if (xcell.col() < ncol_) {
        CellType type = xcell.type(na, wb_.stringTable(), wb_.dateStyles());
        if (type > types[xcell.col()]) {
          types[xcell.col()] = type;
        }
      }
      if (xcell.row() > m) {
        i++;
        m = xcell.row();
      }
      it++;
    }

    return types;
  }

  Rcpp::CharacterVector colNames() {
    Rcpp::CharacterVector out(ncol_);
    std::vector<XlsxCell>::const_iterator it = firstRow_;
    int base = it->row();

    while(it != cells_.end() && it->row() == base) {
      XlsxCell xcell = *it;
      if (xcell.col() >= ncol_) {
        break;
      }
      out[xcell.col()] = xcell.asCharSxp("", wb_.stringTable());
      it++;
    }
    return out;
  }

  Rcpp::List readCols(Rcpp::CharacterVector names,
                      const std::vector<CellType>& types,
                      const StringSet& na,
                      bool has_col_names = false) {

    std::vector<XlsxCell>::const_iterator it;
    it = has_col_names ? secondRow_: firstRow_;

    int base = firstRow_->row() + has_col_names;
    int n = (it == cells_.end()) ? 0 : nrow_ - base;
    Rcpp::List cols(ncol_);
    cols.attr("names") = names;
    for (int j = 0; j < ncol_; ++j) {
      cols[j] = makeCol(types[j], n);
    }

    if (it == cells_.end()) {
      return cols;
    }

    // we have read i rows of data
    int i;
    // account for any empty rows between column headers and data start
    i = it->row() - base;
    // m is the max row number seen so far
    int m = it->row();
    while (it != cells_.end()) {

      if ((i + 1) % 1000 == 0) {
        Rcpp::checkUserInterrupt();
      }

      XlsxCell xcell = *it;
      if (xcell.col() >= ncol_) {
        continue;
      }
      CellType type = xcell.type(na, wb_.stringTable(), wb_.dateStyles());
      Rcpp::RObject col = cols[xcell.col()];
      // row to write into
      int row = xcell.row() - base;
      // Needs to compare to actual cell type to give warnings
      switch(types[xcell.col()]) {
      case CELL_BLANK:
        break;
      case CELL_NUMERIC:
        switch(type) {
        case CELL_NUMERIC:
        case CELL_DATE:
          REAL(col)[row] = xcell.asDouble(na);
          break;
        case CELL_BLANK:
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_TEXT:
          Rcpp::warning("[%i, %i]: expecting numeric: got '%s'",
                        xcell.row() + 1, xcell.col() + 1,
                        xcell.asStdString(wb_.stringTable()));
          REAL(col)[row] = NA_REAL;
        }
        break;
      case CELL_DATE:
        switch(type) {
        case CELL_DATE:
          REAL(col)[row] = xcell.asDate(na, wb_.offset());
          break;
        case CELL_BLANK:
          REAL(col)[row] = NA_REAL;
          break;
        case CELL_NUMERIC:
        case CELL_TEXT:
          Rcpp::warning("[%i, %i]: expecting date: got '%s'",
                        xcell.row() + 1, xcell.col() + 1,
                        xcell.asStdString(wb_.stringTable()));
          REAL(col)[row] = NA_REAL;
          break;
        }
        break;
      case CELL_TEXT:
        if (type == CELL_BLANK) {
          SET_STRING_ELT(col, row, NA_STRING);
        } else {
          SET_STRING_ELT(col, row, xcell.asCharSxp(na, wb_.stringTable()));
        }
        break;
      }
      if (xcell.row() > m) {
        i++;
        m = xcell.row();
      }
      it++;
    }
    return cols;
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
  //   firstRow_ = first cell for which declared row >= nskip
  //   secondRow_ = first cell for which declared row > that of firstRow_
  //   fallback to cells_.end() if the above not possible
  // Assumes loaded cells are arranged s.t. row is non-decreasing
  void parseGeometry(int nskip) {
    nrow_ = 0;
    ncol_ = 0;

    // empty sheet case
    if (cells_.size() == 0) {
      return;
    }

    firstRow_ = cells_.end();
    secondRow_ = cells_.end();
    std::vector<XlsxCell>::const_iterator it = cells_.begin();

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

};

#endif
