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
  XlsxCell(rapidxml::xml_node<>* cell, int row = -1, int col = -1):
  cell_(cell)
  {
    rapidxml::xml_attribute<>* ref = cell_->first_attribute("r");
    if (ref == NULL) {
      location_ = std::make_pair(row, col);
    } else {
      location_ = parseRef(ref->value());
    }
    type_ = CELL_UNKNOWN;
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

  void inferType(const StringSet& na,
                 const std::vector<std::string>& stringTable,
                 const std::set<int>& dateStyles) {
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

    if (type_ != CELL_UNKNOWN) {
      return;
    }

    rapidxml::xml_attribute<>* t = cell_->first_attribute("t");
    rapidxml::xml_node<>* v = cell_->first_node("v");

    // inlineStr (Inline String)  Cell containing an (inline) rich string
    if (t != NULL && strncmp(t->value(), "inlineStr", 9) == 0) {
      // must do this first, because inlineStr cells do not have a v node
      // and the check just below would otherwise make them all CELL_BLANK
      rapidxml::xml_node<>* is = cell_->first_node("is");
      std::string inline_string;
      if (parseString(is, &inline_string)) {
        type_ = na.contains(inline_string) ? CELL_BLANK : CELL_TEXT;
      } else {
        type_ = CELL_BLANK;
      }
      return;
    }

    if (v == NULL || na.contains(v->value())) {
      type_ = CELL_BLANK;
      return;
    }
    // from here on, the only explicit NA check needed is the case of
    // a shared string table lookup

    // n (Number)                 Cell containing a number.
    if (t == NULL || strncmp(t->value(), "n", 5) == 0) {
      rapidxml::xml_attribute<>* s = cell_->first_attribute("s");
      int style = (s == NULL) ? -1 : atoi(s->value());
      type_ = (dateStyles.count(style) > 0) ? CELL_DATE : CELL_NUMERIC;
      return;
    }

    // b (Boolean)                Cell containing a boolean.
    if (strncmp(t->value(), "b", 5) == 0) {
      type_ = CELL_LOGICAL;
      return;
    }

    // d (Date)                   Cell contains a date in the ISO 8601 format.
    if (strncmp(t->value(), "d", 5) == 0) {
      // Hadley:
      // Does excel use this? Regardless, don't have cross-platform ISO8601
      // parser (yet) so need to return as text
      // Jenny:
      // Not entirely sure what this is about. I've never seen one IRL.
      type_ = CELL_TEXT;
      return;
    }

    // e (Error)                  Cell containing an error.
    if (strncmp(t->value(), "e", 5) == 0) {
      type_ = CELL_BLANK;
      return;
    }

    // s (Shared String)          Cell containing a shared string.
    if (strncmp(t->value(), "s", 5) == 0) {
      int id = atoi(v->value());
      const std::string& string = stringTable.at(id);
      type_ = na.contains(string) ? CELL_BLANK : CELL_TEXT;
      return;
    }

    // str (String)               Cell containing a formula string.
    if (strncmp(t->value(), "str", 5) == 0) {
      type_ = CELL_TEXT;
      return;
    }

    Rcpp::warning("Unrecognized cell type at [%i, %i]: '%s'",
                  row() + 1, col() + 1, t->value());
  }

  std::string asStdString(const std::vector<std::string>& stringTable) const {
    rapidxml::xml_node<>* v = cell_->first_node("v");
    rapidxml::xml_attribute<>* t = cell_->first_attribute("t");

    switch(type_) {

    case CELL_UNKNOWN:
    case CELL_BLANK:
      return "";

    case CELL_LOGICAL:
      return atoi(v->value()) ? "TRUE" : "FALSE";

    case CELL_DATE:
      // not ideal for a date but will have to do ... one day: asDateString()?
    case CELL_NUMERIC:
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
        return stringFromTable(v->value(), stringTable);
      }

      //   formula string cell or
      //   the mythical ISO 8601 date cell
      return(v->value());
    }
  }
  }

  Rcpp::RObject asCharSxp(const std::vector<std::string>& stringTable) const {
    std::string out_string = asStdString(stringTable);
    return out_string.empty() ? NA_STRING : Rf_mkCharCE(out_string.c_str(), CE_UTF8);
  }

  int asInteger() const {
    switch(type_) {

    case CELL_UNKNOWN:
    case CELL_BLANK:
    case CELL_DATE:
    case CELL_TEXT:
      return NA_LOGICAL;

    case CELL_LOGICAL:
    case CELL_NUMERIC:
    {
      rapidxml::xml_node<>* v = cell_->first_node("v");
      return atoi(v->value());
    }
    }
  }

  double asDouble() const {
    switch(type_) {

    case CELL_UNKNOWN:
    case CELL_BLANK:
    case CELL_TEXT:
      return NA_REAL;

    case CELL_LOGICAL:
    case CELL_DATE:
    case CELL_NUMERIC:
    {
      rapidxml::xml_node<>* v = cell_->first_node("v");
      return atof(v->value());
    }
    }
  }

  double asDate(int offset) const {
    switch(type_) {

    case CELL_UNKNOWN:
    case CELL_BLANK:
    case CELL_LOGICAL:
    case CELL_TEXT:
      return NA_REAL;

    case CELL_DATE:
    case CELL_NUMERIC:
    {
      rapidxml::xml_node<>* v = cell_->first_node("v");
      return (atof(v->value()) - offset) * 86400;
    }
    }
  }

private:

  std::string stringFromTable(const char* val,
                              const std::vector<std::string>& stringTable) const {
    int id = atoi(val);
    if (id < 0 || id >= (int) stringTable.size()) {
      Rcpp::warning("Invalid string id at [%i, %i]:  %i",
                    row() + 1, col() + 1, id);
      return "";
    }
    const std::string& string = stringTable.at(id);
    return string;
  }

};

#endif
