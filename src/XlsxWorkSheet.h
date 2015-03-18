#ifndef EXELL_XLSXWORKSHEET_
#define EXELL_XLSXWORKSHEET_

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

public:

  XlsxWorkSheet(XlsxWorkBook wb, std::string sheet): wb_(wb) {
    std::string sheetPath = tfm::format("xl/worksheets/%s.xml", sheet);
    sheet_ = zip_buffer(wb.path(), sheetPath);
    sheetXml_.parse<0>(&sheet_[0]);

    rootNode_ = sheetXml_.first_node("worksheet");
    if (rootNode_ == NULL)
      Rcpp::stop("Invalid sheet xml (no <worksheet>)");

    sheetData_ = rootNode_->first_node("sheetData");
    if (sheetData_ == NULL)
      Rcpp::stop("Invalid sheet xml (no <sheetData>)");
  }

  void printCells() {
    for (rapidxml::xml_node<>* row = sheetData_->first_node("row");
         row; row = row->next_sibling("row")) {

      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {

        XlsxCell xcell(cell);
        Rcpp::Rcout << xcell.row() << "," << xcell.row() << ": " <<
          cellTypeDesc(xcell.type("", wb_.dateStyles())) << "\n";
      }
    }
  }


  std::vector<CellType> colTypes(std::string na, int nskip = 0, int n_max = 100) {
    rapidxml::xml_node<>* row = getRow(nskip);
    std::vector<CellType> types;

    int i = 0;
    while(i < n_max && row != NULL) {
      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {

        XlsxCell xcell(cell);
        if (xcell.col() >= types.size()) {
          types.resize(xcell.col() + 1);
        }

        CellType type = xcell.type("", wb_.dateStyles());
        if (type > types[xcell.col()]) {
          types[xcell.col()] = type;
        }
      }

      row = row->next_sibling("row");
      i++;
    }

    return types;
  }

  Rcpp::CharacterVector colNames(int nskip = 0) {
    rapidxml::xml_node<>* row = getRow(nskip);

    int p = 0;
    for (rapidxml::xml_node<>* cell = row->first_node("c");
         cell; cell = cell->next_sibling("c")) {
      p = XlsxCell(cell).col(); // assuming cells always ordered
    }

    Rcpp::CharacterVector out(4);
    for (rapidxml::xml_node<>* cell = row->first_node("c");
         cell; cell = cell->next_sibling("c")) {
      XlsxCell xcell(cell);
      out[xcell.col()] = xcell.asCharSxp("", wb_.strings());
    }

    return out;
  }

  Rcpp::List readCols(Rcpp::CharacterVector names, std::vector<CellType> types,
                      std::string na, int nskip = 0) {
    if (names.size() != types.size())
      Rcpp::stop("Names and types must be same size");

    rapidxml::xml_node<>* firstRow = getRow(nskip);

    // Determine rows and cols
    int p = types.size();
    int n = 0;
    for (rapidxml::xml_node<>* row = firstRow->next_sibling("row");
         row; row = row->next_sibling("row")) {
      n++;
    }

    // Initialise columns
    Rcpp::List cols(p);
    for (int j = 0; j < p; ++j) {
      cols[j] = makeCol(types[j], n);
    }

    int i = 0;
    for (rapidxml::xml_node<>* row = firstRow->next_sibling("row");
         row; row = row->next_sibling("row")) {

      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {

        XlsxCell xcell(cell);
        CellType type = xcell.type(na, wb_.dateStyles());
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
          case CELL_TEXT:
            Rcpp::warning("[%i, %i]: expecting numeric: got `%s`",
              xcell.row() + 1, xcell.col() + 1, xcell.asStdString());
            REAL(col)[i] = NA_REAL;
          }
          break;
        case CELL_DATE:
          switch(type) {
          case CELL_DATE:
            REAL(col)[i] = xcell.asDate(na, 0);
            break;
          case CELL_BLANK:
          case CELL_NUMERIC:
          case CELL_TEXT:
            Rcpp::warning("[%i, %i]: expecting date: got '%s'",
              xcell.row() + 1, xcell.col() + 1, xcell.asStdString());
            REAL(col)[i] = NA_REAL;
            break;
          }
          break;
        case CELL_TEXT:
          if (type == CELL_BLANK) {
            SET_STRING_ELT(col, i, NA_STRING);
          } else {
            SET_STRING_ELT(col, i, xcell.asCharSxp(na, wb_.strings()));
          }
          break;
        }
      }

      ++i;
    }

    return colDataframe(cols, names, types);
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
};

#endif
