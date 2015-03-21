#include <Rcpp.h>

#include "zip.h"
#include "rapidxml.h"
#include "rapidxml_print.h"

using namespace Rcpp;

std::string zip_buffer(const std::string& zip_path,
                       const std::string& file_path) {
  Rcpp::Environment env = Rcpp::Environment("package:readxl");
  Rcpp::Function zip_buffer = env["zip_buffer"];

  Rcpp::RawVector xml = Rcpp::as<Rcpp::RawVector>(zip_buffer(zip_path, file_path));
  std::string buffer(RAW(xml), RAW(xml) + xml.size());
  buffer.push_back('\0');

  return buffer;
}

// // [Rcpp::export]
// std::string xml_print(std::string xml) {
//   rapidxml::xml_document<> doc;
//
//   xml.push_back('\0');
//   doc.parse<0>(&xml[0]);
//
//   std::ostringstream out;
//   rapidxml::print(out, doc, 0);
//
//   return out.str();
// }
