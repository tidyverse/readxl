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
  // JB QUESTION: seems like a worksheet should know its own name (and/or
  // position). This could be used in messaging, such as, "SHEET has no data"
  std::string sheet_;
  rapidxml::xml_document<> sheetXml_;
  rapidxml::xml_node<>* rootNode_;
  rapidxml::xml_node<>* sheetData_;
  std::vector<XlsxCell> cells_;
  int ncol_, nrow_, nskip_;
  std::vector<XlsxCell>::const_iterator firstRow_, secondRow_;

public:

  XlsxWorkSheet(XlsxWorkBook wb, int sheet_i, int nskip):
  wb_(wb)
  {
    if (sheet_i > wb.n_sheets()) {
      Rcpp::stop("Can't retrieve sheet in position %d, only %d sheets found.",
                 sheet_i,  wb.n_sheets());
    }
    std::string sheetPath = wb.sheetPath(sheet_i);
    sheet_ = zip_buffer(wb.path(), sheetPath);
    sheetXml_.parse<0>(&sheet_[0]);

    rootNode_ = sheetXml_.first_node("worksheet");
    if (rootNode_ == NULL)
      Rcpp::stop("Invalid sheet xml (no <worksheet>)");

    sheetData_ = rootNode_->first_node("sheetData");
    if (sheetData_ == NULL)
      Rcpp::stop("Invalid sheet xml (no <sheetData>)");

    nskip_ = nskip;
    loadCells();
    cacheDimension();
    markRows();
  }

  int ncol() {
    return ncol_;
  }

  int nrow() {
    return nrow_;
  }

  // JB: this should either take colNames as an argument or have a bit of code
  // moved out of here, so we don't read column names again inside this fxn.
  // More comments near end of fxn.
  std::vector<CellType> colTypes(const StringSet& na,
                                 int n_max = 100, bool has_col_names = false) {
    std::vector<CellType> types;
    types.resize(ncol_);

    std::vector<XlsxCell>::const_iterator it;
    if (has_col_names) {
      it = secondRow_;
    } else {
      it = firstRow_;
    }

    // no cell data to consult re: types
    if (it == cells_.end()) {
      for (size_t i = 0; i < types.size(); i++) {
        types[i] = CELL_NUMERIC;
      }
      return types;
    }

    int base = firstRow_->row() + has_col_names;
    std::vector<XlsxCell>::const_iterator row_end;

    // we have consulted i rows re: determining col types
    int i;
    // account for any empty rows between column headers and data start
    i = it->row() - base;

    while (i < n_max && it != cells_.end()) {
      // find the end of current row
      row_end = it;
      while(row_end != cells_.end() && row_end->row() == it->row()) {
        row_end++;
      }

      while (it != row_end) {
        XlsxCell xcell = *it;
        if (xcell.col() < ncol_) {
          CellType type = xcell.type(na, wb_.stringTable(), wb_.dateStyles());
          if (type > types[xcell.col()]) {
            types[xcell.col()] = type;
          }
        }
        it++;
      }

      i++;
    }

    // JB: this usually rederives the column names, which seems unwise
    // I propose to move such reconciliation of column names and types out
    // of here and into the parent function , read_xlsx_()
    if (has_col_names) {
      // blank columns with a name aren't blank
      Rcpp::CharacterVector names = colNames();
      for (size_t i = 0; i < types.size(); i++) {
        if (types[i] == CELL_BLANK && names[i] != NA_STRING && names[i] != "")
          types[i] = CELL_NUMERIC;
      }
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
    // JB: suspect this should move out of here and into a function that does
    // this and the last rationalization re col names and types in colTypes
    if ((int) names.size() != ncol_ || (int) types.size() != ncol_)
      Rcpp::stop("Need one name and type for each column");

    std::vector<XlsxCell>::const_iterator it;
    if (has_col_names) {
      it = secondRow_;
    } else {
      it = firstRow_;
    }

    // no cell data to read
    if (it == cells_.end()) {
      Rcpp::List cols(ncol_);
      for (int j = 0; j < ncol_; ++j) {
        cols[j] = makeCol(types[j], 0);
      }
      return removeBlankColumns(cols, names, types);
    }

    int base = firstRow_->row() + has_col_names;
    std::vector<XlsxCell>::const_iterator row_end;

    // we have read i rows of data
    int i;
    // account for any empty rows between column headers and data start
    i = it->row() - base;

    // Initialise columns, accounting for leading skipped or blank rows
    int n = nrow_ - base;
    Rcpp::List cols(ncol_);
    for (int j = 0; j < ncol_; ++j) {
      cols[j] = makeCol(types[j], n);
    }

    while (it != cells_.end()) {

      if ((i + 1) % 1000 == 0) {
        Rcpp::checkUserInterrupt();
      }

      // find the end of this row
      row_end = it;
      while(row_end != cells_.end() && row_end->row() == it->row()) {
        row_end++;
      }

      while (it != row_end) {
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
        it++;
      }
      ++i;
    }

    return removeBlankColumns(cols, names, types);
  }


private:

  void cacheDimension() {
    // 18.3.1.35 dimension (Worksheet Dimensions) [p 1627]
    rapidxml::xml_node<>* dimension = rootNode_->first_node("dimension");
    if (dimension == NULL) {
      return computeDimensions();
    }

    rapidxml::xml_attribute<>* ref = dimension->first_attribute("ref");
    if (ref == NULL) {
      return computeDimensions();
    }

    const char* refv = ref->value();
    while (*refv != ':' && *refv != '\0')
      ++refv;
    if (*refv == '\0') {
      return computeDimensions();
    }

    ++refv; // advanced past :
    std::pair<int, int> dim = parseRef(refv);
    if (dim.first == -1 || dim.second == -1) {
      return computeDimensions();
    }

    nrow_ = dim.first + 1; // size is one greater than max position
    ncol_ = dim.second + 1;
  }

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
        XlsxCell xcell(cell, i, j);
        cells_.push_back(xcell);
        j++;
      }
      i++;
    }

  }

  // If <dimension> not present, consult location data stored in cells
  void computeDimensions() {
    nrow_ = 0;
    ncol_ = 0;

    // empty sheet case
    if (cells_.size() == 0) {
      return;
    }
    for (std::vector<XlsxCell>::const_iterator it = cells_.begin();
         it != cells_.end(); ++it) {
      XlsxCell xcell = *it;
      if (nrow_ < xcell.row()) {
        nrow_ = xcell.row();
      }
      if (ncol_ < xcell.col()) {
        ncol_ = xcell.col();
      }
    }
    nrow_++;
    ncol_++;
  }

  // Position iterators at various landmarks for reading:
  // firstRow_ = first cell for which declared row >= nskip
  // secondRow_ = first cell for which declared row > that of firstRow_
  // fallback value is cells_.end() if the above not possible
  void markRows() {
    // empty sheet case
    if (cells_.size() == 0) {
      return;
    }

    firstRow_ = cells_.end();
    secondRow_ = cells_.end();

    std::vector<XlsxCell>::const_iterator it = cells_.begin();
    while (it != cells_.end() && it->row() < nskip_) {
      it++;
    }
    if (it == cells_.end()) {
      return;
    }
    firstRow_ = it++;

    while (it != cells_.end()) {
      if (it->row() < firstRow_->row()) {
        secondRow_ = firstRow_;
        firstRow_ = it;
      } else if (it->row() > firstRow_->row() &&
        (secondRow_ == cells_.end() || it->row() < secondRow_->row())) {
        secondRow_ = it;
      }
      ++it;
    }
  }

};

#endif
