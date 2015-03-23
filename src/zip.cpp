#include <Rcpp.h>

#include "zip.h"
#include "rapidxml_print.h"

using namespace Rcpp;

Function readxl(const std::string& fun){
  Environment env = Environment::namespace_env("readxl");
  return env[fun];
}

std::string zip_buffer(const std::string& zip_path,
                       const std::string& file_path) {
  Rcpp::Function zip_buffer = readxl("zip_buffer");

  Rcpp::RawVector xml = Rcpp::as<Rcpp::RawVector>(zip_buffer(zip_path, file_path));
  std::string buffer(RAW(xml), RAW(xml) + xml.size());
  buffer.push_back('\0');

  return buffer;
}

bool zip_has_file(const std::string& zip_path,
                  const std::string& file_path) {
  Rcpp::Function zip_has_file = readxl("zip_has_file");

  LogicalVector res = wrap<LogicalVector>(zip_has_file(zip_path, file_path));
  return res[0];
}

std::string xml_print(std::string xml) {
  rapidxml::xml_document<> doc;

  xml.push_back('\0');
  doc.parse<0>(&xml[0]);

  std::string s;
  rapidxml::print(std::back_inserter(s), doc, 0);

  return s;
}

// [[Rcpp::export]]
void zip_xml(const std::string& zip_path,
             const std::string& file_path) {

  std::string buffer = zip_buffer(zip_path, file_path);
  Rcout << xml_print(buffer);
}

