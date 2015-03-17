#include <Rcpp.h>
using namespace Rcpp;
#include "rapidxml.h"
using namespace rapidxml;

// [[Rcpp::export]]
std::vector<std::string> xlsx_sheets_(RawVector xml) {
  std::string buffer;
  buffer.resize(xml.size() + 1);
  memcpy(&buffer[0], RAW(xml), xml.size());
  buffer[buffer.size() - 1] = '\0';

  xml_document<> doc;
  doc.parse<0>(&buffer[0]);

  std::vector<std::string> sheetNames;
  xml_node<>* sheets = doc.first_node()->first_node("sheets");

  for (xml_node<>* sheet = sheets->first_node(); sheet;
       sheet = sheet->next_sibling()) {
    std::string value(sheet->first_attribute("name")->value());
    sheetNames.push_back(value);
  }

  return sheetNames;
}
