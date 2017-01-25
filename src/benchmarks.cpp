#include <Rcpp.h>
#include "rapidxml.h"
#include "XlsxWorkBook.h"
using namespace Rcpp;

// [[Rcpp::export]]
void parseXml(std::string base, std::string internal) {
  std::string file = zip_buffer(base, internal);
  Rcout << "File size: " << file.size() << " bytes\n";

  rapidxml::xml_document<> xml;
  xml.parse<0>(&file[0]);
}

// [[Rcpp::export]]
int countRows(std::string base, int sheet) {
  // FYI: sheet lookup logic has changed in main package
  // this is not generally correct
  std::string sheetPath = tfm::format("xl/worksheets/sheet%i.xml", sheet + 1);

  std::string file = zip_buffer(base, sheetPath);
  Rcout << "File size: " << file.size() << " bytes\n";

  rapidxml::xml_document<> xml;
  xml.parse<0>(&file[0]);

  rapidxml::xml_node<>* rootNode_ = xml.first_node("worksheet");
  if (rootNode_ == NULL)
    return 0;

  rapidxml::xml_node<>* sheetData_ = rootNode_->first_node("sheetData");
  if (sheetData_ == NULL)
    return 0;

  int i = 0;
  for (rapidxml::xml_node<>* row = sheetData_->first_node("row");
       row; row = row->next_sibling("row")) {
    i++;
  }

  return i;
}
