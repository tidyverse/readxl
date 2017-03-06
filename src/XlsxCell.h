#ifndef READXL_XLSXCELL_
#define READXL_XLSXCELL_

#include <Rcpp.h>
#include "rapidxml.h"
#include "ColSpec.h"
#include "XlsxString.h"
#include "utils.h"

// Key reference for understanding the structure of the XML is
// ECMA-376 (http://www.ecma-international.org/publications/standards/Ecma-376.htm)
// Section and page numbers below refer to the 4th edition
// 18.3.1.4   c           (Cell)       [p1598]
// 18.3.1.96  v           (Cell Value) [p1709]
// 18.18.11   ST_CellType (Cell Type)  [p2443]

class XlsxCell {
  rapidxml::xml_node<>* cell_;
  std::pair<int,int> location_;
  CellType type_;

public:

  // if possible, provide guess at row and column based on position within xml
  XlsxCell(rapidxml::xml_node<>* cell,
           const std::vector<std::string>& stringTable,
           const std::set<int>& dateStyles,
           const StringSet& na = "",
           int row = -1, int col = -1):
  cell_(cell)
  {
    rapidxml::xml_attribute<>* ref = cell->first_attribute("r");
    if (ref == NULL) {
      location_ = std::make_pair(row, col);
    } else {
      location_ = parseRef(ref->value());
    }
    type_ = inferType(na, stringTable, dateStyles);
  }

  int row() const {
    return location_.first;
  }

  int col() const {
    return location_.second;
  }

  CellType type() const {
    return type_;
  }

  std::string asStdString(const std::vector<std::string>& stringTable) const {
    rapidxml::xml_node<>* v = cell_->first_node("v");
    rapidxml::xml_attribute<>* t = cell_->first_attribute("t");

    switch(type_) {

    case CELL_BLANK:
      return "NA";

    case CELL_LOGICAL:
      return atoi(v->value()) ? "TRUE" : "FALSE";

    case CELL_DATE:
    case CELL_NUMERIC:
      // not ideal for a date but will have to do ... one day: asDateString()?
      return std::string(v->value());

    case CELL_TEXT:
    {
      std::string out_string;

      // inlineStr
      rapidxml::xml_node<>* is = cell_->first_node("is");
      if (is != NULL) {
        return parseString(is, &out_string) ? out_string : "NA";
      }

      // shared string
      if (strncmp(t->value(), "s", 5) == 0) {
        int id = atoi(v->value());
        if (id < 0 || id >= (int) stringTable.size()) {
          Rcpp::warning("Invalid string id at [%i, %i]: %i",
                        row() + 1, col() + 1, id);
          return "NA";
        }
        return(stringTable.at(id));
      }

      //   the mythical ISO 8601 date cell
      //   formula string cell
      return(v->value());
    }
    }
  }

  int asInteger() const {
    rapidxml::xml_node<>* v = cell_->first_node("v");
    return v == NULL ? NA_LOGICAL : atoi(v->value());
  }

  double asDouble() const {
    rapidxml::xml_node<>* v = cell_->first_node("v");
    return v == NULL ? NA_REAL : atof(v->value());
  }

  double asDate(int offset) const {
    rapidxml::xml_node<>* v = cell_->first_node("v");
    return v == NULL ? NA_REAL : (atof(v->value()) - offset) * 86400;
  }

  Rcpp::RObject asCharSxp(const std::vector<std::string>& stringTable) const {

    // Is it an inline string?  // 18.3.1.53 is (Rich Text Inline) [p1649]
    rapidxml::xml_node<>* is = cell_->first_node("is");
    if (is != NULL) {
      std::string value;
      if (parseString(is, &value)) {
        return Rf_mkCharCE(value.c_str(), CE_UTF8);
      } else {
        return NA_STRING;
      }
    }

    rapidxml::xml_node<>* v = cell_->first_node("v");
    if (v == NULL) {
      return NA_STRING;
    }

    rapidxml::xml_attribute<>* t = cell_->first_attribute("t");

    if (t != NULL && strncmp(t->value(), "s", t->value_size()) == 0) {
      return stringFromTable(v->value(), stringTable);
    } else {
      return Rf_mkCharCE(v->value(), CE_UTF8);
    }
  }

private:

  CellType inferType(const StringSet& na,
                     const std::vector<std::string>& stringTable,
                     const std::set<int>& dateStyles) const {
    // 1. Review of Excel's declared cell types, then
    // 2. Summary of how Excel's cell types map to our CellType enum
    //
    // this table refers to the value of the t attribute of a cell
    // 18.18.11   ST_CellType (Cell Type)  [p2443]
    // This simple type is restricted to the values listed in the following table:
    // -------------------------------------------------------------------------
    // Enumeration Value          Description
    // -------------------------------------------------------------------------
    // b (Boolean)                Cell containing a boolean.
    // d (Date)                   Cell contains a date in the ISO 8601 format.
    // e (Error)                  Cell containing an error.
    // inlineStr (Inline String)  Cell containing an (inline) rich string, i.e.,
    //                            one not in the shared string table. If this
    //                            cell type is used, then the cell value is in
    //                            the is element rather than the v element in
    //                            the cell (c element).
    // n (Number)                 Cell containing a number.
    // s (Shared String)          Cell containing a shared string.
    // str (String)               Cell containing a formula string.
    //
    // We map Excel's cell types to the CellType enum based on declared type
    // and contents.
    //
    // CELL_BLANK
    //   inlineStr cell and (string is na or string can't be found)
    //   cell has no v node and is not an inlineStr cell
    //   v->value() is na
    //   error cell
    //   shared string cell and string is na
    //
    // CELL_LOGICAL
    //   Boolean cell and its value (TRUE or FALSE) is not in na
    //
    // CELL_DATE
    //   numeric cell (t attr is "n" or does not exist) with a date style
    //
    // CELL_NUMERIC
    //   numeric cell (t attr is "n" or does not exist) with no style or a
    //   non-date style
    //
    // CELL_TEXT
    //   inlineStr cell and string is found and string is not na
    //   ISO 8601 date cell (t attr is "d") <- we're not sure this exists IRL
    //   shared string cell and string is not na
    //   formula string cell and string is not na
    //   anything that is not explicitly addressed elsewhere

    rapidxml::xml_attribute<>* t = cell_->first_attribute("t");
    rapidxml::xml_node<>* v = cell_->first_node("v");

    // inlineStr (Inline String)  Cell containing an (inline) rich string
    if (t != NULL && strncmp(t->value(), "inlineStr", 9) == 0) {
      // must do this first, because inlineStr cells do not have a v node
      // and the check just below would otherwise make them all CELL_BLANK
      rapidxml::xml_node<>* is = cell_->first_node("is");
      std::string inline_string;
      if (parseString(is, &inline_string)) {
        return na.contains(inline_string) ? CELL_BLANK : CELL_TEXT;
      } else {
        return CELL_BLANK;
      }
    }

    if (v == NULL || na.contains(v->value())) {
      return CELL_BLANK;
    }
    // from here on, the only explicit NA check needed is the case of
    // a shared string table lookup

    // n (Number)                 Cell containing a number.
    if (t == NULL || strncmp(t->value(), "n", 5) == 0) {
      rapidxml::xml_attribute<>* s = cell_->first_attribute("s");
      int style = (s == NULL) ? -1 : atoi(s->value());
      return (dateStyles.count(style) > 0) ? CELL_DATE : CELL_NUMERIC;
    }

    // b (Boolean)                Cell containing a boolean.
    if (strncmp(t->value(), "b", 5) == 0) {
      return CELL_LOGICAL;
    }

    // d (Date)                   Cell contains a date in the ISO 8601 format.
    if (strncmp(t->value(), "d", 5) == 0) {
      // Hadley:
      // Does excel use this? Regardless, don't have cross-platform ISO8601
      // parser (yet) so need to return as text
      // Jenny:
      // Not entirely sure what this is about. I've never seen one IRL.
      return CELL_TEXT;
    }

    // e (Error)                  Cell containing an error.
    if (strncmp(t->value(), "e", 5) == 0) {
      return CELL_BLANK;
    }

    // s (Shared String)          Cell containing a shared string.
    if (strncmp(t->value(), "s", 5) == 0) {
      int id = atoi(v->value());
      const std::string& string = stringTable.at(id);
      return na.contains(string) ? CELL_BLANK : CELL_TEXT;
    }

    // str (String)               Cell containing a formula string.
    if (strncmp(t->value(), "str", 5) == 0) {
      return CELL_TEXT;
    }

    Rcpp::warning("Unrecognized cell type at [%i, %i]: '%s'",
                  row() + 1, col() + 1, t->value());

    return CELL_TEXT;
  }

  Rcpp::RObject stringFromTable(const char* val,
                                const std::vector<std::string>& stringTable) const {
    int id = atoi(val);
    if (id < 0 || id >= (int) stringTable.size()) {
      Rcpp::warning("[%i, %i]: Invalid string id %i", row() + 1, col() + 1, id);
      return NA_STRING;
    }

    const std::string& string = stringTable.at(id);
    return Rf_mkCharCE(string.c_str(), CE_UTF8);
  }

};

#endif
