#ifndef EXELL_XLSXWORKSHEET_
#define EXELL_XLSXWORKSHEET_

#include <Rcpp.h>
#include "rapidxml.h"
#include "CellType.h"
#include "XlsxWorkBook.h"

// Simple parser: does not check that order of numbers and letters is correct
inline std::pair<int, int> parseRef(std::string ref) {
  int col = 0, row = 0;

  for (std::string::iterator cur = ref.begin(); cur != ref.end(); ++cur) {
    if (*cur >= '0' && *cur <= '9') {
      row = row * 10 + (*cur - '0');
    } else if (*cur >= 'A' && *cur <= 'Z') {
      col = 26 * col + (*cur - 'A' + 1);
    } else {
      Rcpp::stop("Invalid character ('%s') in cell ref", *cur);
    }
  }

  return std::make_pair(row, col);
}

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

public:

  XlsxWorkSheet(XlsxWorkBook wb, std::string sheet): wb_(wb) {
    std::string sheetPath = tfm::format("xl/worksheets/%s.xml", sheet);
    std::string sheet_ = zip_buffer(wb.path(), sheetPath);
    sheetXml_.parse<0>(&sheet_[0]);

    rapidxml::xml_node<>* rootNode = sheetXml_.first_node("worksheet");
    if (rootNode == NULL)
      Rcpp::stop("Invalid sheet xml (no <worksheet>)");

    sheetData_ = rootNode->first_node("sheetData");
    if (sheetData_ == NULL)
      Rcpp::stop("Invalid sheet xml (no <sheetData>)");
  }

  void printCells() {
    for (rapidxml::xml_node<>* row = sheetData_->first_node("row");
         row; row = row->next_sibling("row")) {

      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {

        rapidxml::xml_attribute<>* ref = cell->first_attribute("r");
        if (ref == NULL) {
          Rcpp::Rcout << "unknown";
        } else {
          std::pair<int,int> parsed = parseRef(std::string(ref->value()));
          Rcpp::Rcout << parsed.first << ", " << parsed.second << "\n";
        }
      }
    }
  }
};

#endif
