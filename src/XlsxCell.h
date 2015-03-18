#ifndef EXELL_XLSXCELL_
#define EXELL_XLSXCELL_

#include <Rcpp.h>
#include "rapidxml.h"
#include "CellType.h"

// Key reference for understanding the structure of the XML is
// ECMA-376 (http://www.ecma-international.org/publications/standards/Ecma-376.htm)
// Section and page numbers below refer to the 4th edition
// 18.3.1.4   c           (Cell)       [p1598]
// 18.3.1.96  v           (Cell Value) [p1709]
// 18.18.11   ST_CellType (Cell Type)  [p2443]

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

class XlsxCell {
  rapidxml::xml_node<>* cell_;
  std::pair<int,int> location_;

public:

  XlsxCell(rapidxml::xml_node<>* cell): cell_(cell) {
    rapidxml::xml_attribute<>* ref = cell_->first_attribute("r");
    if (ref == NULL)
      Rcpp::stop("Invalid cell: lacks ref attribute");

    location_ = parseRef(std::string(ref->value()));
  }

  int row() {
    return location_.first;
  }

  int col() {
    return location_.second;
  }

  std::string typeString() {
    rapidxml::xml_attribute<>* t = cell_->first_attribute("t");
    return (t == NULL) ? "n" : std::string(t->value());
  }

  CellType type(const std::set<int>& dateStyles) {
    std::string type = typeString();

    if (type == "b") {
      // TODO
      return CELL_NUMERIC;
    } else if (type == "d") {
      // Does excel use this? Regardless, don't have cross-platform ISO8601
      // parser (yet) so need to return as text
      return CELL_TEXT;
    } else if (type == "n") {
      rapidxml::xml_attribute<>* s = cell_->first_attribute("s");
      int style = (s == NULL) ? -1 : atoi(s->value());

      return (dateStyles.count(style) > 0) ? CELL_DATE : CELL_NUMERIC;
    } else if (type == "s" || type == "str") {
      return CELL_TEXT;
    } else {
      // I don't think Excel uses inline strings ("inlineStr")
      Rcpp::warning("Unknown type '%s' in [%i, %i]", type, row(), col());
      return CELL_TEXT;
    }

    return CELL_NUMERIC;
  }

};

inline std::string cellRef(rapidxml::xml_node<>* cell) {
  rapidxml::xml_attribute<>* ref = cell->first_attribute("r");
  return (ref == NULL) ? "??" : std::string(ref->value());
}

inline CellType cellType(rapidxml::xml_node<>* cell, std::set<int> dateStyles) {
  rapidxml::xml_attribute<>* t = cell->first_attribute("t");
  std::string type = (t == NULL) ? "n" : std::string(t->value());

  if (type == "b") {
    // TODO
    return CELL_NUMERIC;
  } else if (type == "d") {
    // Does excel use this? Regardless, don't have cross-platform ISO8601
    // parser (yet) so need to return as text
    return CELL_TEXT;
  } else if (type == "n") {
    rapidxml::xml_attribute<>* s = cell->first_attribute("s");
    int style = (s == NULL) ? -1 : atoi(s->value());

    return (dateStyles.count(style) > 0) ? CELL_DATE : CELL_NUMERIC;
  } else if (type == "s" || type == "str") {
    return CELL_TEXT;
  } else {
    // I don't think Excel uses inline strings ("inlineStr")
    Rcpp::warning("Unknown type '%s' in cell '%s'", type, cellRef(cell));
  }

  return CELL_NUMERIC;
}


#endif
