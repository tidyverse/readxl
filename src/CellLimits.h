#ifndef READXL_CELLLIMITS_
#define READXL_CELLLIMITS_

#include <Rcpp.h>
#include "XlsCell.h"

class CellLimits {
  int minRow_, maxRow_, minCol_, maxCol_;

public:
  CellLimits() {
    minRow_ = -1;
    maxRow_ = -1;
    minCol_ = -1;
    maxCol_ = -1;
  }
  CellLimits(Rcpp::IntegerVector limits) {
    minRow_ = limits[0];
    maxRow_ = limits[1];
    minCol_ = limits[2];
    maxCol_ = limits[3];
  }

  int minRow() const {
    return minRow_;
  }
  int maxRow() const {
    return maxRow_;
  }
  int minCol() const {
    return minCol_;
  }
  int maxCol() const {
    return maxCol_;
  }

  void update(const XlsCell cell) {
    update(cell.row(), cell.col());
  }

  void update(const int row, const int col) {
    if (minRow_ < 0 || row < minRow_) {
      minRow_ = row;
    }
    if (row > maxRow_) {
      maxRow_ = row;
    }
    if (minCol_ < 0 || col < minCol_) {
      minCol_ = col;
    }
    if (col > maxCol_) {
      maxCol_ = col;
    }
  }

  bool contains(const XlsCell cell) const {
    return contains(cell.row(), cell.col());
  }

  bool contains(const int i, const int j) const {
    return contains(minRow_, maxRow_, i) && contains(minCol_, maxCol_, j);
  }

  bool contains(const int i) const {
    return contains(minRow_, maxRow_, i);
  }

  void print() {
    Rcpp::Rcout << "row min, max: " << minRow_ << ", "
                << maxRow_ << "\t"
                << "col min, max: " << minCol_<< ", "
                << maxCol_ << std::endl;
  }

private:

  bool contains(int min, int max, int val) const {
    if (min < 0) {
      if (max < 0) {
        // min = max = -1 is our convention for 'no limits specified'
        return true;
      } else {
        // min < 0, max >= 0 should never happen, because cellranger should
        // always turn (-inf, max] into [0, max], but it's harmless to handle
        return val <= max;
      }
    } else {
      if (max < 0) {
        // min >= 0, max < 0 is our convention for [min, +inf)
        return val >= min;
      } else {
        // min >= 0, max >= 0 is the straightforward case: [min, max]
        return val >= min && val <= max;
      }
    }
  }
};

#endif
