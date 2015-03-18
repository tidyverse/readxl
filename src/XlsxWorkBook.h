#ifndef EXELL_XLSXWORKBOOK_
#define EXELL_XLSXWORKBOOK_

#include <Rcpp.h>
#include "rapidxml.h"
#include "CellType.h"
#include "utils.h"

inline std::string zip_buffer(std::string zip_path, std::string file_path) {
  Rcpp::Environment exellEnv = Rcpp::Environment("package:exell");
  Rcpp::Function zip_buffer = exellEnv["zip_buffer"];

  Rcpp::RawVector xml = Rcpp::as<Rcpp::RawVector>(zip_buffer(zip_path, file_path));
  std::string buffer(RAW(xml), RAW(xml) + xml.size());
  buffer.push_back('\0');

  return buffer;
}

class XlsxWorkBook {
  std::string path_;
  std::set<int> dateStyles_;
  double offset_;


public:

  XlsxWorkBook(std::string path): path_(path) {
    offset_ = dateOffset(is1904());
    cacheDateStyles();
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

  std::string path() {
    return path_;
  }

  std::set<int> dateStyles() {
    return dateStyles_;
  }

  double offset() {
    return offset_;
  }

private:

  void cacheDateStyles() {
    std::string sharedStringsXml = zip_buffer(path_, "xl/styles.xml");
    rapidxml::xml_document<> sharedStrings;
    sharedStrings.parse<0>(&sharedStringsXml[0]);

    rapidxml::xml_node<>* styleSheet = sharedStrings.first_node("styleSheet");
    if (styleSheet == NULL)
      return;

    // Figure out which custom formats are dates
    std::set<int> customDateFormats;
    rapidxml::xml_node<>* numFmts = styleSheet->first_node("numFmts");
    if (numFmts != NULL) {
      for (rapidxml::xml_node<>* numFmt = numFmts->first_node();
           numFmt; numFmt = numFmt->next_sibling()) {
        std::string code(numFmt->first_attribute("formatCode")->value());
        int id = atoi(numFmt->first_attribute("numFmtId")->value());

        if (isDateFormat(code))
          customDateFormats.insert(id);
      }
    }

    // Cache styles that have date formatting
    rapidxml::xml_node<>* cellXfs = styleSheet->first_node("cellXfs");
    if (cellXfs == NULL)
      return;

    int i = 0;
    for (rapidxml::xml_node<>* cellXf = cellXfs->first_node();
         cellXf; cellXf = cellXf->next_sibling()) {
      int formatId = atoi(cellXf->first_attribute("numFmtId")->value());
      if (isDateTime(formatId, customDateFormats))
        dateStyles_.insert(i);
      ++i;
    }
  }

  bool is1904() {
    std::string workbookXml = zip_buffer(path_, "xl/workbook.xml");
    rapidxml::xml_document<> workbook;
    workbook.parse<0>(&workbookXml[0]);

    rapidxml::xml_node<>* root = workbook.first_node("workbook");
    if (root == NULL)
      return false;

    rapidxml::xml_node<>* workbookPr = root->first_node("workbookPr");
    if (workbookPr == NULL)
      return false;

    rapidxml::xml_attribute<>* date1904 = workbookPr->first_attribute("date1904");
    if (date1904 == NULL)
      return false;

    return atoi(date1904->value()) == 1;
  }

};

#endif
