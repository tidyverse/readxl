#include "WingAndPrayer.hpp"

#include "cpp11/as.hpp"
#include "cpp11/R.hpp"
#include "cpp11/strings.hpp"

#include <string>

template <typename T>
cpp11::strings read_this_(
    std::string path,
    int sheet_i,
    cpp11::integers limits,
    bool shim,
    cpp11::sexp col_names,
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
  bool has_col_names = false;
  switch(TYPEOF(col_names)) {
  case STRSXP:
    colNames = cpp11::writable::strings(static_cast<SEXP>(col_names));
    break;
  case LGLSXP:
    has_col_names = cpp11::as_cpp<bool>(col_names);
    colNames = has_col_names ? ws.colNames(na, trim_ws) : cpp11::writable::strings(ws.ncol());
    break;
  default:
    cpp11::stop("`col_names` must be a logical or character vector");
  }
  return colNames;
}

[[cpp11::register]]
cpp11::strings read_this_xls_(
    std::string path,
    int sheet_i,
    cpp11::integers limits,
    bool shim,
    cpp11::sexp col_names,
    std::vector<std::string> na,
    bool trim_ws,
    bool progress) {
  return read_this_<Xls>(path, sheet_i, limits, shim, col_names, na, trim_ws, progress);
}

[[cpp11::register]]
cpp11::strings read_this_xlsx_(
    std::string path,
    int sheet_i,
    cpp11::integers limits,
    bool shim,
    cpp11::sexp col_names,
    std::vector<std::string> na,
    bool trim_ws,
    bool progress) {
  return read_this_<Xlsx>(path, sheet_i, limits, shim, col_names, na, trim_ws, progress);
}
