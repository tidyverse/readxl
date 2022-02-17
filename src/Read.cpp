#include "WingAndPrayer.hpp"

#include <string>

template <typename T>
void read_this_(std::string path, int sheet_i) {
  Sheet<T> ws(path, sheet_i);
  ws.commonMethod();
}

[[cpp11::register]]
void read_this_xls_(std::string path, int sheet_i) {
  read_this_<Xls>(path, sheet_i);
}

[[cpp11::register]]
void read_this_xlsx_(std::string path, int sheet_i) {
  read_this_<Xlsx>(path, sheet_i);
}

