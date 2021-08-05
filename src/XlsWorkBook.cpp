#include "XlsWorkBook.h"
#include <cassert> //for assert

[[cpp11::register]]
cpp11::writable::strings xls_sheets(std::string path) {
  XlsWorkBook wb(path);
  return wb.sheets();
}

[[cpp11::register]]
std::set<int> xls_date_formats(std::string path) {
  return XlsWorkBook(path).dateFormats();
}
