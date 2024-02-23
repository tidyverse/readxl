#pragma once

#include "ColSpec.h"
#include "XlsxString.h"
#include "utils.h"

#include "rapidxml/rapidxml.h"

// Key reference for understanding the structure of the XML is
// ECMA-376 (http://www.ecma-international.org/publications/standards/Ecma-376.htm)
// Section and page numbers below refer to the 5th edition October 2016
// 18.2.5     definedName  (Defined Name) [p1553]
// 18.2.6     definedNames (Defined Name) [p1558]

class XlsxDefinedName {
  rapidxml::xml_node<>* definedName_;
  std::string name_;

public:

  XlsxCell(rapidxml::xml_node<>* definedName):
  definedName_(definedName)
  {
    rapidxml::xml_attribute<>* attr = cell_->first_attribute("name");
    name_ = (attr != NULL) ? Rf_mkCharCE(attr->value(), CE_UTF8) : NA_STRING;
  }

  std::string name() const {
    return name_;
  }

};
