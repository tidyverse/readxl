#define R_NO_REMAP
#define STRICT_R_HEADERS

#ifndef READXL_CELLLIMITS_
#define READXL_CELLLIMITS_

#include <cpp11/integers.hpp>
#include <cpp11/doubles.hpp>
#include "XlsCell.h"

class CellLimits {
  double minRow_, maxRow_, minCol_, maxCol_;

public:
  CellLimits() {
    minRow_ = -1;
    maxRow_ = -1;
    minCol_ = -1;
    maxCol_ = -1;
  }
  CellLimits(cpp11::doubles limits) {
    minRow_ = limits[0];
    maxRow_ = limits[1];
    minCol_ = limits[2];
    maxCol_ = limits[3];
  }

  double minRow() const {
    return minRow_;
  }
  double maxRow() const {
    return maxRow_;
  }
  double minCol() const {
    return minCol_;
  }
  double maxCol() const {
    return maxCol_;
  }

  void update(const XlsCell cell) {
    update(cell.row(), cell.col());
  }

  void update(const double row, const double col) {
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

  void update(const double minRow, const double maxRow,
              const double minCol, const double maxCol) {
    minRow_ = minRow;
    maxRow_ = maxRow;
    minCol_ = minCol;
    maxCol_ = maxCol;
  }

  bool contains(const XlsCell cell) const {
    return contains(cell.row(), cell.col());
  }

  bool contains(const double i, const double j) const {
    return contains(minRow_, maxRow_, i) && contains(minCol_, maxCol_, j);
  }

  bool contains(const double i) const {
    return contains(minRow_, maxRow_, i);
  }

  void print() {
    Rprintf("row min, max: %d, %d \t col min, max: %d, %d",
            minRow_, maxRow_, minCol_, maxCol_);
  }
private:

  bool contains(double min, double max, double val) const {
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
