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
  rapidxml::xml_node<>* rootNode_;
  rapidxml::xml_node<>* sheetData_;
  int ncol_, nrow_;

public:

  XlsxWorkSheet(XlsxWorkBook wb, int i): wb_(wb) {
    std::string sheetPath = tfm::format("xl/worksheets/sheet%i.xml", i + 1);
    sheet_ = zip_buffer(wb.path(), sheetPath);
    sheetXml_.parse<0>(&sheet_[0]);

    rootNode_ = sheetXml_.first_node("worksheet");
    if (rootNode_ == NULL)
      Rcpp::stop("Invalid sheet xml (no <worksheet>)");

    sheetData_ = rootNode_->first_node("sheetData");
    if (sheetData_ == NULL)
      Rcpp::stop("Invalid sheet xml (no <sheetData>)");

    cacheDimension();
  }

  int ncol() {
    return ncol_;
  }

  int nrow() {
    return nrow_;
  }

  void printCells() {
    for (rapidxml::xml_node<>* row = sheetData_->first_node("row");
         row; row = row->next_sibling("row")) {

      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {

        XlsxCell xcell(cell);
        Rcpp::Rcout << xcell.row() << "," << xcell.row() << ": " <<
          cellTypeDesc(xcell.type("", wb_.stringTable(), wb_.dateStyles())) << "\n";
      }
    }
  }


  std::vector<CellType> colTypes(const StringSet& na, int nskip = 0, int n_max = 100, bool has_col_names = false) {
    rapidxml::xml_node<>* row = getRow(nskip + has_col_names);
    std::vector<CellType> types;
    types.resize(ncol_);

    int i = 0;
    while(i < n_max && row != NULL) {
      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {

        XlsxCell xcell(cell);
        if (xcell.col() >= ncol_)
          continue;

        CellType type = xcell.type(na, wb_.stringTable(), wb_.dateStyles());
        if (type >= types[xcell.col()]) {
          types[xcell.col()] = type;
        }
      }

      row = row->next_sibling("row");
      i++;
    }

    if (has_col_names) {
      // blank columns with a name aren't blank
      Rcpp::CharacterVector names = colNames(nskip);
      for (size_t i = 0; i < types.size(); i++) {
        if (types[i] == CELL_BLANK && names[i] != NA_STRING && names[i] != "")
          types[i] = CELL_NUMERIC;
      }
    }

    return types;
  }

  Rcpp::CharacterVector colNames(int nskip = 0) {
    rapidxml::xml_node<>* row = getRow(nskip);

    Rcpp::CharacterVector out(ncol_);
    for (rapidxml::xml_node<>* cell = row->first_node("c");
         cell; cell = cell->next_sibling("c")) {
      XlsxCell xcell(cell);

      if (xcell.col() >= ncol_)
        continue;
      out[xcell.col()] = xcell.asCharSxp("", wb_.stringTable());
    }

    return out;
  }

  Rcpp::List readCols(Rcpp::CharacterVector names,
                      const std::vector<CellType>& types,
                      const StringSet& na, int nskip = 0) {
    if ((int) names.size() != ncol_ || (int) types.size() != ncol_)
      Rcpp::stop("Need one name and type for each column");

    // Initialise columns
    int n = nrow_ - nskip;
    Rcpp::List cols(ncol_);
    for (int j = 0; j < ncol_; ++j) {
      cols[j] = makeCol(types[j], n);
    }

    int i = 0;
    for (rapidxml::xml_node<>* row = getRow(nskip);
         row; row = row->next_sibling("row")) {
      if ((i + 1) % 1000 == 0)
        Rcpp::checkUserInterrupt();

      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {

        XlsxCell xcell(cell);
        CellType type = xcell.type(na, wb_.stringTable(), wb_.dateStyles());
        if (xcell.col() >= ncol_)
          continue;

        Rcpp::RObject col = cols[xcell.col()];
        // Needs to compare to actual cell type to give warnings
        switch(types[xcell.col()]) {
        case CELL_BLANK:
          break;
        case CELL_NUMERIC:
          switch(type) {
          case CELL_NUMERIC:
          case CELL_DATE:
            REAL(col)[i] = xcell.asDouble(na);
            break;
          case CELL_BLANK:
            REAL(col)[i] = NA_REAL;
            break;
          case CELL_TEXT:
            Rcpp::warning("[%i, %i]: expecting numeric: got '%s'",
              xcell.row() + 1, xcell.col() + 1, xcell.asStdString(wb_.stringTable()));
            REAL(col)[i] = NA_REAL;
          }
          break;
        case CELL_DATE:
          switch(type) {
          case CELL_DATE:
            REAL(col)[i] = xcell.asDate(na, wb_.offset());
            break;
          case CELL_BLANK:
            REAL(col)[i] = NA_REAL;
            break;
          case CELL_NUMERIC:
          case CELL_TEXT:
            Rcpp::warning("[%i, %i]: expecting date: got '%s'",
              xcell.row() + 1, xcell.col() + 1, xcell.asStdString(wb_.stringTable()));
            REAL(col)[i] = NA_REAL;
            break;
          }
          break;
        case CELL_TEXT:
          if (type == CELL_BLANK) {
            SET_STRING_ELT(col, i, NA_STRING);
          } else {
            SET_STRING_ELT(col, i, xcell.asCharSxp(na, wb_.stringTable()));
          }
          break;
        }
      }

      ++i;
    }

    return removeBlankColumns(cols, names, types);
  }


private:

  rapidxml::xml_node<>* getRow(int i) {
    rapidxml::xml_node<>* row = sheetData_->first_node("row");
    while(i > 0 && row != NULL) {
      row = row->next_sibling("row");
      i--;
    }
    if (row == NULL)
      Rcpp::stop("Skipped over all data");

    return row;
  }

  void cacheDimension() {
    // 18.3.1.35 dimension (Worksheet Dimensions) [p 1627]
    rapidxml::xml_node<>* dimension = rootNode_->first_node("dimension");
    if (dimension == NULL)
      return computeDimensions();

    rapidxml::xml_attribute<>* ref = dimension->first_attribute("ref");
    if (ref == NULL)
      return computeDimensions();

    const char* refv = ref->value();
    while (*refv != ':' && *refv != '\0')
      ++refv;
    if (*refv == '\0')
      return computeDimensions();

    ++refv; // advanced past :
    std::pair<int, int> dim = parseRef(refv);
    if (dim.first == -1 || dim.second == -1)
      return computeDimensions();

    nrow_ = dim.first + 1; // size is one greater than max position
    ncol_ = dim.second + 1;
  }

  void computeDimensions() {
    // If <dimension> not present, iterate over all rows and cells to count
    nrow_ = 0;
    ncol_ = 0;

    for (rapidxml::xml_node<>* row = sheetData_->first_node("row");
      row; row = row->next_sibling("row")) {

      for (rapidxml::xml_node<>* cell = row->first_node("c");
        cell; cell = cell->next_sibling("c")) {

        XlsxCell xcell(cell);
        if (nrow_ < xcell.row())
          nrow_ = xcell.row();

        if (ncol_ < xcell.col())
          ncol_ = xcell.col();

      }
    }
    nrow_++;
    ncol_++;
  }

};

#endif
