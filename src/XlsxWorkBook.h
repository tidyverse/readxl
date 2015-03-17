#include <Rcpp.h>
#include "rapidxml.h"

inline std::string zip_buffer(std::string zip_path, std::string file_path) {
  Rcpp::Environment exellEnv = Rcpp::Environment("package:exell");
  Rcpp::Function zip_buffer = exellEnv["zip_buffer"];

  RawVector xml = Rcpp::as<RawVector>(zip_buffer(zip_path, file_path));
  std::string buffer(RAW(xml), RAW(xml) + xml.size());
  buffer.push_back('\0');

  return buffer;
}

class XlsxWorkBook {
  std::string path_;

public:

  XlsxWorkBook(std::string path): path_(path) {
  }

  std::vector<std::string> sheets() {
    std::string workbookXml = zip_buffer(path_, "xl/workbook.xml");
    rapidxml::xml_document<> workbook;
    workbook.parse<0>(&workbookXml[0]);

    std::vector<std::string> sheetNames;

    rapidxml::xml_node<>* root = workbook.first_node("workbook");
    if (root == NULL)
      return sheetNames;

    rapidxml::xml_node<>* sheets = root->first_node("sheets");
    if (sheets == NULL)
      return sheetNames;

    for (rapidxml::xml_node<>* sheet = sheets->first_node();
         sheet; sheet = sheet->next_sibling()) {
      std::string value(sheet->first_attribute("name")->value());
      sheetNames.push_back(value);
    }

    return sheetNames;
  }


  std::vector<std::string> strings() {
    std::string sharedStringsXml = zip_buffer(path_, "xl/sharedStrings.xml");
    rapidxml::xml_document<> sharedStrings;
    sharedStrings.parse<0>(&sharedStringsXml[0]);

    std::vector<std::string> strings;

    rapidxml::xml_node<>* sst = sharedStrings.first_node("sst");
    if (sst == NULL)
      return strings;

    rapidxml::xml_attribute<>* count = sst->first_attribute("count");
    if (count != NULL) {
      int n = atoi(count->value());
      strings.reserve(n);
    }

    for (rapidxml::xml_node<>* string = sst->first_node();
         string; string = string->next_sibling()) {
      std::string value(string->first_node("t")->value());
      strings.push_back(value);
    }

    return strings;
  }

};
