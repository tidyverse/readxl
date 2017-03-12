#ifndef UTILS_
#define UTILS_

#include <cerrno>
#include "StringSet.h"

// 18.2.28 workbookPr (Workbook Properties) p1582
// date1904:
// Value that indicates whether to use a 1900 or 1904 date system when
// converting serial date-times in the workbook to dates.
// A value of 1 or true indicates the workbook uses the 1904 date system.
// A value of 0 or false indicates the workbook uses the 1900 date system. (See
// 18.17.4.1 for the definition of the date systems.)
// The default value for this attribute is false.
inline double dateOffset(bool is1904) {
  // as.numeric(as.Date("1899-12-30"))
  // as.numeric(as.Date("1904-01-01"))
  return is1904 ? 24107 : 25569;
}

// this is sort of horrible
// convert serial date to decimilliseconds
// use well-known hack to round to nearest integer w/o C++11 or boost, e.g.
// http://stackoverflow.com/questions/485525/round-for-float-in-c
// convert back to serial date
inline double dateRound(double dttm) {
  double ms = dttm * 10000;
  ms = (ms >= 0.0 ? std::floor(ms + 0.5) : std::ceil(ms - 0.5));
  return ms / 10000;
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
      Rcpp::stop("Invalid character '%s' in cell ref '%s'", *cur, ref);
    }
  }

  return std::make_pair(row - 1, col - 1); // zero indexed
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

#endif
