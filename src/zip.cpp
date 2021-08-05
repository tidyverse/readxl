#include "zip.h"
#include "rapidxml_print.h"

#include <cpp11/function.hpp>
#include <cpp11/raws.hpp>

std::string zip_buffer(const std::string& zip_path,
                       const std::string& file_path) {
  cpp11::function zip_buffer = cpp11::package("readxl")["zip_buffer"];

  cpp11::raws xml(zip_buffer(zip_path, file_path));
  std::string buffer(RAW(xml), RAW(xml) + xml.size());
  buffer.push_back('\0');

  return buffer;
}

bool zip_has_file(const std::string& zip_path,
                  const std::string& file_path) {
  cpp11::function zip_has_file = cpp11::package("readxl")["zip_has_file"];
  return zip_has_file(zip_path, file_path);
}

std::string xml_print(std::string xml) {
  rapidxml::xml_document<> doc;

  xml.push_back('\0');
  doc.parse<rapidxml::parse_strip_xml_namespaces>(&xml[0]);

  std::string s;
  rapidxml::print(std::back_inserter(s), doc, 0);

  return s;
}

[[cpp11::register]]
void zip_xml(const std::string& zip_path,
             const std::string& file_path) {

  std::string buffer = zip_buffer(zip_path, file_path);
  Rprintf("%s", xml_print(buffer).c_str());
}
