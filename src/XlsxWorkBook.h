#ifndef READXL_XLSXWORKBOOK_
#define READXL_XLSXWORKBOOK_

#include <Rcpp.h>
#include "rapidxml.h"
#include "CellType.h"
#include "XlsxString.h"
#include "utils.h"
#include "zip.h"

class XlsxWorkBook {
  std::string path_;
  std::set<int> dateStyles_;
  std::vector<std::string> stringTable_;
  double offset_;

public:

  XlsxWorkBook(const std::string& path): path_(path) {
    offset_ = dateOffset(is1904());
    cacheStringTable();
    cacheDateStyles();
  }

  Rcpp::CharacterVector sheets() {
    std::string workbookXml = zip_buffer(path_, "xl/workbook.xml");
    rapidxml::xml_document<> workbook;
    workbook.parse<0>(&workbookXml[0]);

    int n = 100;
    Rcpp::CharacterVector sheetNames(n);

    rapidxml::xml_node<>* root = workbook.first_node("workbook");
    if (root == NULL) {
      return sheetNames;
    }

    rapidxml::xml_node<>* sheets = root->first_node("sheets");
    if (sheets == NULL) {
      return sheetNames;
    }

    int i = 0;
    for (rapidxml::xml_node<>* sheet = sheets->first_node();
         sheet; sheet = sheet->next_sibling()) {
      if (i >= n) {
        n *= 2;
        sheetNames = Rf_lengthgets(sheetNames, n);
      }
      rapidxml::xml_attribute<>* name = sheet->first_attribute("name");
      sheetNames[i] = (name != NULL) ? Rf_mkCharCE(name->value(), CE_UTF8) : NA_STRING;
      i++;
    }

    if (i != n) {
      sheetNames = Rf_lengthgets(sheetNames, i);
    }

    return sheetNames;
  }


  const std::string& path() {
    return path_;
  }

  const std::set<int>& dateStyles() {
    return dateStyles_;
  }

  const std::vector<std::string>& stringTable() {
    return stringTable_;
  }

  double offset() {
    return offset_;
  }

private:

  void cacheStringTable() {
    if (!zip_has_file(path_, "xl/sharedStrings.xml")) {
      return;
    }

    std::string sharedStringsXml = zip_buffer(path_, "xl/sharedStrings.xml");
    rapidxml::xml_document<> sharedStrings;
    sharedStrings.parse<0>(&sharedStringsXml[0]);

    rapidxml::xml_node<>* sst = sharedStrings.first_node("sst");
    if (sst == NULL) {
      return;
    }

    rapidxml::xml_attribute<>* count = sst->first_attribute("count");
    if (count != NULL) {
      int n = atoi(count->value());
      stringTable_.reserve(n);
    }

    // 18.4.8 si (String Item) [p1725]
    for (rapidxml::xml_node<>* string = sst->first_node();
         string; string = string->next_sibling()) {
      std::string out;
      parseString(string, &out);    // missing strings are treated as empty
      stringTable_.push_back(out);
    }
  }

  void cacheDateStyles() {
    std::string sharedStringsXml = zip_buffer(path_, "xl/styles.xml");
    rapidxml::xml_document<> sharedStrings;
    sharedStrings.parse<0>(&sharedStringsXml[0]);

    rapidxml::xml_node<>* styleSheet = sharedStrings.first_node("styleSheet");
    if (styleSheet == NULL) {
      return;
    }

    // Figure out which custom formats are dates
    std::set<int> customDateFormats;
    rapidxml::xml_node<>* numFmts = styleSheet->first_node("numFmts");
    if (numFmts != NULL) {
      for (rapidxml::xml_node<>* numFmt = numFmts->first_node();
           numFmt; numFmt = numFmt->next_sibling()) {
        std::string code(numFmt->first_attribute("formatCode")->value());
        int id = atoi(numFmt->first_attribute("numFmtId")->value());

        if (isDateFormat(code)) {
          customDateFormats.insert(id);
        }
      }
    }

    // Cache styles that have date formatting
    rapidxml::xml_node<>* cellXfs = styleSheet->first_node("cellXfs");
    if (cellXfs == NULL) {
      return;
    }

    int i = 0;
    for (rapidxml::xml_node<>* cellXf = cellXfs->first_node();
         cellXf; cellXf = cellXf->next_sibling()) {
      if (cellXf->first_attribute("numFmtId") != NULL) {
        int formatId = atoi(cellXf->first_attribute("numFmtId")->value());
        if (isDateTime(formatId, customDateFormats))
          dateStyles_.insert(i);
        ++i;
      }
    }
  }

  bool is1904() {
    std::string workbookXml = zip_buffer(path_, "xl/workbook.xml");
    rapidxml::xml_document<> workbook;
    workbook.parse<0>(&workbookXml[0]);

    rapidxml::xml_node<>* root = workbook.first_node("workbook");
    if (root == NULL) {
      return false;
    }

    rapidxml::xml_node<>* workbookPr = root->first_node("workbookPr");
    if (workbookPr == NULL) {
      return false;
    }

    rapidxml::xml_attribute<>* date1904 = workbookPr->first_attribute("date1904");
    if (date1904 == NULL) {
      return false;
    }

    return atoi(date1904->value()) == 1;
  }

};

#endif
