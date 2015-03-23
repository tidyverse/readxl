#include <Rcpp.h>

#include "zip.h"
#include "rapidxml_print.h"

using namespace Rcpp;

std::string zip_buffer(const std::string& zip_path,
                       const std::string& file_path) {
  Rcpp::Environment env = Rcpp::Environment::namespace_env("readxl");
  Rcpp::Function zip_buffer = env["zip_buffer"];

  Rcpp::RawVector xml = Rcpp::as<Rcpp::RawVector>(zip_buffer(zip_path, file_path));
  std::string buffer(RAW(xml), RAW(xml) + xml.size());
  buffer.push_back('\0');

  return buffer;
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

