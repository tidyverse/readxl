#include <cpp11/function.hpp>
#include <cpp11/raws.hpp>
#include <cpp11/logicals.hpp>
#include <cpp11/as.hpp>
#include <Rcpp.h>
#include "zip.h"
#include "rapidxml_print.h"

cpp11::function readxl(const std::string& fun){
  auto env = cpp11::package("readxl")[fun];
  return env;
}

std::string zip_buffer(const std::string& zip_path,
                       const std::string& file_path) {
  cpp11::function zip_buffer = readxl("zip_buffer");

  cpp11::writable::raws xml = cpp11::as_cpp<cpp11::writable::raws>(zip_buffer(zip_path, file_path));
  std::string buffer(RAW(xml), RAW(xml) + xml.size());
  buffer.push_back('\0');

  return buffer;
}

bool zip_has_file(const std::string& zip_path,
                  const std::string& file_path) {
  cpp11::function zip_has_file = readxl("zip_has_file");

  cpp11::writable::logicals res = cpp11::as_cpp<cpp11::writable::logicals>(zip_has_file(zip_path, file_path));
  return cpp11::r_bool(res[0]);
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
  Rcpp::Rcout << xml_print(buffer);
}
