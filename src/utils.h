#ifndef UTILS_
#define UTILS_

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

#endif
