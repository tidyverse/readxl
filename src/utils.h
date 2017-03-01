#ifndef UTILS_
#define UTILS_

#include "StringSet.h"

inline double dateOffset(bool is1904) {
  // as.numeric(as.Date("1899-12-30"))
  // as.numeric(as.Date("1904-01-01"))
  return is1904 ? 24107 : 25569;
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
  static const std::string trues [] = {"T", "TRUE", "True", "true"};
  static const std::string falses [] = {"F", "FALSE", "False", "false"};
  std::vector<std::string> true_strings(
      trues,
      trues + (sizeof(trues)/sizeof(std::string))
  );
  std::vector<std::string> false_strings(
      falses,
      falses + (sizeof(falses)/sizeof(std::string))
  );
  StringSet true_values(true_strings);
  StringSet false_values(false_strings);
  if (true_values.contains(maybe_tf)) {
    *out = true;
    matches = true;
  }
  if (false_values.contains(maybe_tf)) {
    *out = false;
    matches = true;
  }
  return matches;
}

#endif
