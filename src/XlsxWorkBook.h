#ifndef READXL_XLSXWORKBOOK_
#define READXL_XLSXWORKBOOK_

#include <Rcpp.h>
#include "rapidxml.h"
#include "ColSpec.h"
#include "XlsxString.h"
#include "utils.h"
#include "zip.h"

class XlsxWorkBook {

  // holds objects related to sheet position, name, Id, and xml target file
  class SheetRelations {
    int n_;
    Rcpp::CharacterVector names_;
    Rcpp::CharacterVector id_;
    std::map<std::string, std::string> target_;

    // populates n_, names_, id_
    void parse_workbook(const std::string& path) {
      std::string workbookXml = zip_buffer(path, "xl/workbook.xml");
      rapidxml::xml_document<> workbook;
      workbook.parse<rapidxml::parse_strip_xml_namespaces>(&workbookXml[0]);

      rapidxml::xml_node<>* root = workbook.first_node("workbook");
      if (root == NULL) {
        return;
      }

      rapidxml::xml_node<>* sheets = root->first_node("sheets");
      if (sheets == NULL) {
        return;
      }

      int i = 0;
      for (rapidxml::xml_node<>* sheet = sheets->first_node();
           sheet; sheet = sheet->next_sibling()) {
        if (i >= n_) {
          n_ *= 2;
          names_ = Rf_lengthgets(names_, n_);
          id_ = Rf_lengthgets(id_, n_);
        }
        rapidxml::xml_attribute<>* name = sheet->first_attribute("name");
        names_[i] = (name != NULL) ? Rf_mkCharCE(name->value(), CE_UTF8) : NA_STRING;

        rapidxml::xml_attribute<>* id = sheet->first_attribute("id");
        id_[i] = (id != NULL) ? Rf_mkCharCE(id->value(), CE_UTF8) : NA_STRING;

        i++;
      }

      if (i != n_) {
        names_ = Rf_lengthgets(names_, i);
        id_ = Rf_lengthgets(id_, i);
        n_ = i;
      }
    }

    // populates target_
    void parse_workbook_rels(const std::string& path) {
      std::string rels_xml_file = zip_buffer(path, "xl/_rels/workbook.xml.rels");
      rapidxml::xml_document<> rels_xml;
      rels_xml.parse<rapidxml::parse_strip_xml_namespaces>(&rels_xml_file[0]);

      rapidxml::xml_node<>* relationships = rels_xml.first_node("Relationships");
      if (relationships == NULL) {
        return;
      }

      for (rapidxml::xml_node<>* relationship = relationships->first_node();
           relationship; relationship = relationship->next_sibling()) {
        rapidxml::xml_attribute<>* id = relationship->first_attribute("Id");
        rapidxml::xml_attribute<>* target = relationship->first_attribute("Target");
        if (id != NULL && target != NULL) {
          static const std::string prefix = "/xl/";
          std::string target_value = target->value();
          if (target_value.substr(0, prefix.size()) == prefix) {
            target_value = target_value.substr(prefix.size());
          }
          target_[id->value()] = target_value;
        }
      }
    }

  public:
    SheetRelations(const std::string& path) :
    n_(100),
    names_(n_),
    id_(n_)
    {
      parse_workbook(path);
      parse_workbook_rels(path);
    }

    Rcpp::CharacterVector names() const {
      return names_;
    }

    int n_sheets() const {
      return n_;
    }

    std::string target(int sheet_i) const {
      std::string id = Rcpp::as<std::string>(id_[sheet_i]);
      std::map<std::string, std::string>::const_iterator it = target_.find(id);
      if (it == target_.end()) {
        Rcpp::stop("`%s` not found", id);
      }
      return it->second;
    }
  }; // end of class SheetRelations

  // common to Xls[x]WorkBook
  std::string path_;
  bool is1904_;
  std::set<int> dateFormats_;

  // specific to XlsxWorkBook
  SheetRelations rel_;
  std::vector<std::string> stringTable_;

public:

  XlsxWorkBook(const std::string& path):
  path_(path),
  rel_(path)
  {
    is1904_ = uses1904();
    cacheStringTable();
    cacheDateFormats();
  }

  const std::string& path() const{
    return path_;
  }

  int n_sheets() const {
    return rel_.n_sheets();
  }

  Rcpp::CharacterVector sheets() const {
    return rel_.names();
  }

  bool is1904() const {
    return is1904_;
  }

  const std::set<int>& dateFormats() const {
    return dateFormats_;
  }

  std::string sheetPath(int sheet_i) const {
    return "xl/" + rel_.target(sheet_i);
  }

  const std::vector<std::string>& stringTable() const {
    return stringTable_;
  }

private:

  void cacheStringTable() {
    if (!zip_has_file(path_, "xl/sharedStrings.xml")) {
      return;
    }

    std::string sharedStringsXml = zip_buffer(path_, "xl/sharedStrings.xml");
    rapidxml::xml_document<> sharedStrings;
    sharedStrings.parse<rapidxml::parse_strip_xml_namespaces>(&sharedStringsXml[0]);

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

  void cacheDateFormats() {
    std::string stylesXml = zip_buffer(path_, "xl/styles.xml");
    rapidxml::xml_document<> styles;
    styles.parse<rapidxml::parse_strip_xml_namespaces>(&stylesXml[0]);

    rapidxml::xml_node<>* styleSheet = styles.first_node("styleSheet");
    if (styleSheet == NULL) {
      return;
    }

    // Figure out which custom formats are dates
    // 18.8.31 numFmts (Number Formats) p1793 5th ed
    std::set<int> customDateFormats;
    rapidxml::xml_node<>* numFmts = styleSheet->first_node("numFmts");
    if (numFmts != NULL) {
      // Example with just one custom format (non date, in this case)
      // <numFmts count="1">
      //   <numFmt numFmtId="166" formatCode="&quot;$&quot;#,##0.00"/>
      // </numFmts>
      for (rapidxml::xml_node<>* numFmt = numFmts->first_node();
           numFmt; numFmt = numFmt->next_sibling()) {
        // formatCode: The number format code for this number format.
        std::string code(numFmt->first_attribute("formatCode")->value());
        //   numFmtId: Id used by the master style records (xf's) to reference
        //             this number format.
        int id = atoi(numFmt->first_attribute("numFmtId")->value());

        if (isDateFormat(code)) {
          customDateFormats.insert(id);
        }
      }
    }

    // Cache 0-based indices of the master cell style records that refer to a
    // number format that is a date format
    rapidxml::xml_node<>* cellXfs = styleSheet->first_node("cellXfs");
    if (cellXfs == NULL) {
      return;
    }

    // inspecting i-th child of cellXfs = i-th cell style record = an <xf>
    int i = 0;
    for (rapidxml::xml_node<>* cellXf = cellXfs->first_node();
         cellXf; cellXf = cellXf->next_sibling()) {
      if (cellXf->first_attribute("numFmtId") == NULL) {
        ++i;
        continue;
      }
      int formatId = atoi(cellXf->first_attribute("numFmtId")->value());
      if (isDateTime(formatId, customDateFormats))
        dateFormats_.insert(i);
      ++i;
    }
  }

  bool uses1904() {
    std::string workbookXml = zip_buffer(path_, "xl/workbook.xml");
    rapidxml::xml_document<> workbook;
    workbook.parse<rapidxml::parse_strip_xml_namespaces>(&workbookXml[0]);

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
