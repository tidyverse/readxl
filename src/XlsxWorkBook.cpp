#include "XlsxWorkBook.h"

[[cpp11::register]]
cpp11::writable::strings xlsx_sheets(std::string path) {
  return XlsxWorkBook(path).sheets();
}

[[cpp11::register]]
std::vector<std::string> xlsx_strings(std::string path) {
  return XlsxWorkBook(path).stringTable();
}

[[cpp11::register]]
std::set<int> xlsx_date_formats(std::string path) {
  return XlsxWorkBook(path).dateFormats();
}
