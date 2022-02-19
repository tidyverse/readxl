#pragma once

#include "cpp11/protect.hpp"

#include <cerrno>
#include <cmath>
#include <sstream>

//May appear in cpp11
template <typename T, typename N>
T new_vector(R_xlen_t size, N val) {
  T out(size);
  std::fill(out.begin(), out.end(), val);
  return out;
}

// The date offset needed to align Excel dates with R's use of 1970-01-01
// depends on the "date system".
//
// xls ------------------------------------------------------------------------
// Page and section numbers below refer to
// [MS-XLS]: Excel Binary File Format (.xls) Structure
// version, date, and download URL given in XlsCell.h
//
// 2.4.77 Date1904 p257 ... it boils down to a boolean
// 0 --> 1900 date system
// 1 --> 1904 date system
//
// xlsx ------------------------------------------------------------------------
// Page and section numbers below refer to
// ECMA-376
// version, date, and download URL given in XlsxCell.h
//
// 18.2.28 workbookPr (Workbook Properties) p1586
// in xl/workbook.xml, node workbook, child node workbookPr
// attribute date1904:
// 0 or false --> 1900 date system
// 1 or true --> 1904 date system (this is the default)
//
// 18.17.4.1 p2073 holds definition of the date systems
//
// Date systems ---------------------------------------------------------------
// 1900 system: first possible date is 1900-01-01 00:00:00,
//              which has serial value of **1**
// 1904 system: origin 1904-01-01 00:00:00
inline double dateOffset(bool is1904) {
  // as.numeric(as.Date("1899-12-30"))
  // as.numeric(as.Date("1904-01-01"))
  return is1904 ? 24107 : 25569;
}

// this is sort of horrible
// convert serial date to milliseconds
// use well-known hack to round to nearest integer w/o C++11 or boost, e.g.
// http://stackoverflow.com/questions/485525/round-for-float-in-c
// convert back to serial date
inline double dateRound(double dttm) {
  double ms = dttm * 1000;
  ms = (ms >= 0.0 ? std::floor(ms + 0.5) : std::ceil(ms - 0.5));
  return ms / 1000;
}

// this is even more horrible
// correct for Excel's faithful re-implementation of the Lotus 1-2-3 bug,
// in which February 29, 1900 is included in the date system, even though 1900
// was not actually a leap year
// https://support.microsoft.com/en-us/help/214326/excel-incorrectly-assumes-that-the-year-1900-is-a-leap-year
// How we address this:
// If date is *prior* to the non-existent leap day: add a day
// If date is on the non-existent leap day: make negative and, in due course, NA
// Otherwise: do nothing
inline double POSIXctFromSerial(double xlDate, bool is1904) {
  if (!is1904 && xlDate < 61) {
    xlDate = (xlDate < 60) ? xlDate + 1 : -1;
  }
  if (xlDate < 0) {
    Rf_warning("NA inserted for impossible 1900-02-29 datetime");
    return NA_REAL;
  } else {
    return dateRound((xlDate - dateOffset(is1904)) * 86400);
  }
}

// Simple parser: does not check that order of numbers and letters is correct
inline std::pair<int, int> parseRef(const char* ref) {
  int col = 0, row = 0;

  for (const char* cur = ref; *cur != '\0'; ++cur) {
    if (*cur >= '0' && *cur <= '9') {
      row = row * 10 + (*cur - '0');
    } else if (*cur >= 'A' && *cur <= 'Z') {
      col = 26 * col + (*cur - 'A' + 1);
    } else {
      cpp11::stop("Invalid character '%s' in cell ref '%s'", *cur, ref);
    }
  }

  return std::make_pair(row - 1, col - 1); // zero indexed
}

// col = 1 --> first column aka column A, so 1-indexed
inline std::string intToABC(int col) {
  std::string ret;
  while (col > 0) {
    col--;
    ret = (char)('A' + col % 26) + ret;
    col /= 26;
  }
  return ret;
}

// row = 1, col = 1 --> upper left cell aka column A1, so 1-indexed
inline std::string asA1(const int row, const int col) {
  std::ostringstream ret;
  ret << intToABC(col) << row;
  return ret.str();
}

// expects 0-indexed row and col
inline std::string cellPosition(const int row, const int col) {
  std::ostringstream ret;
  ret << asA1(row + 1, col + 1) << " / R" << row + 1 << "C" << col + 1;
  return ret.str();
}

inline bool logicalFromString(std::string maybe_tf, bool *out) {
  bool matches = false;
  if (Rf_StringTrue(maybe_tf.c_str())) {
    *out = true;
    matches = true;
  } else if (Rf_StringFalse(maybe_tf.c_str())) {
    *out = false;
    matches = true;
  }
  return matches;
}

inline bool doubleFromString(std::string mystring, double& out) {
  char* e;
  errno = 0;
  out = std::strtod(mystring.c_str(), &e);
  if (*e != '\0' ||  // error, we didn't consume the entire string
      errno != 0 ) { // error, overflow or underflow
    return false;
  }
  return true;
}

inline std::string trim(const std::string& s) {
  size_t begin = s.find_first_not_of(" \t");
  if (begin == std::string::npos) {
    return "";
  }
  size_t end = s.find_last_not_of(" \t");
  return s.substr(begin, end - begin + 1);
}

inline std::string dirName(const std::string& path) {
  std::size_t found = path.find_last_of('/');
  if (found == std::string::npos) {
    return "";
  }
  return path.substr(0, found);
}

inline std::string baseName(const std::string& path) {
  std::size_t found = path.find_last_of('/');
  if (found == std::string::npos) {
    return path;
  }
  return path.substr(found + 1);
}

inline std::string removeLeadingSlashes(const std::string& s) {
  size_t start = s.find_first_not_of('/');
  if (start == std::string::npos) {
    return "";
  }
  return s.substr(start);
}
