#include "WingAndPrayer.hpp"

#include "cpp11/strings.hpp"

#include <string>

template <typename T>
cpp11::strings read_this_(
    std::string path,
    int sheet_i,
    cpp11::integers limits,
    bool shim,
    std::vector<std::string> na,
    bool trim_ws,
    bool progress) {
  // Construct worksheet ----------------------------------------------
  Sheet<T> ws(path, sheet_i, limits, shim, progress);

  // catches empty sheets and sheets where requested rectangle contains no data
  if (ws.nrow() == 0 && ws.ncol() == 0) {
    using namespace cpp11::literals;
    //return cpp11::writable::list (0_xl);
    return cpp11::strings();
  }

  // Get column names -------------------------------------------------
  cpp11::writable::strings colNames;
  colNames = ws.colNames(na, trim_ws);
  return colNames;
}

[[cpp11::register]]
cpp11::strings read_this_xls_(
    std::string path,
    int sheet_i,
    cpp11::integers limits,
    bool shim,
    std::vector<std::string> na,
    bool trim_ws,
    bool progress) {
  return read_this_<Xls>(path, sheet_i, limits, shim, na, trim_ws, progress);
}

[[cpp11::register]]
cpp11::strings read_this_xlsx_(
    std::string path,
    int sheet_i,
    cpp11::integers limits,
    bool shim,
    std::vector<std::string> na,
    bool trim_ws,
    bool progress) {
  return read_this_<Xlsx>(path, sheet_i, limits, shim, na, trim_ws, progress);
}
