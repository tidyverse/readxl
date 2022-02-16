#include "WingAndPrayer.hpp"

#include <string>

template <typename T>
void read_this_(std::string path) {
  Sheet<T> ws(path);
  ws.commonMethod();
}

[[cpp11::register]]
void read_this_xls_(std::string path) {
  read_this_<Xls>(path);
}

[[cpp11::register]]
void read_this_xlsx_(std::string path) {
  read_this_<Xlsx>(path);
}

