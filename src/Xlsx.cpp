#include <Rcpp.h>
using namespace Rcpp;
#include "rapidxml.h"
using namespace rapidxml;

void parse_raw(xml_document<>* pDoc, RawVector xml) {
  std::string buffer;
  buffer.resize(xml.size() + 1);
  memcpy(&buffer[0], RAW(xml), xml.size());
  buffer[buffer.size() - 1] = '\0';

  pDoc->parse<0>(&buffer[0]);
}

// [[Rcpp::export]]
std::vector<std::string> xlsx_sheets_(RawVector xml) {
  xml_document<> doc;
  parse_raw(&doc, xml);

  std::vector<std::string> sheetNames;
  xml_node<>* sheets = doc.first_node()->first_node("sheets");

  if (sheets == NULL)
    stop("Invalid xlsx file: no workbook/sheets tag found");

  for (xml_node<>* sheet = sheets->first_node(); sheet;
       sheet = sheet->next_sibling()) {
    std::string value(sheet->first_attribute("name")->value());
    sheetNames.push_back(value);
  }

  return sheetNames;
}

// [[Rcpp::export]]
std::vector<std::string> xlsx_strings_(RawVector xml) {
  xml_document<> doc;
  parse_raw(&doc, xml);

  std::vector<std::string> sheetNames;
  xml_node<>* sst = doc.first_node("sst");
  if (sst == NULL)
    stop("Invalid xlsx file: no sst tag found");

  int n = atoi(sst->first_attribute("count")->value());
  std::vector<std::string> strings;
  strings.reserve(n);

  for (xml_node<>* string = sst->first_node(); string;
       string = string->next_sibling()) {
    std::string value(string->first_node("t")->value());
    strings.push_back(value);
  }

  return strings;
}


