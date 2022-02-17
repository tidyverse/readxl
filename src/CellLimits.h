#ifndef READXL_CELLLIMITS_
#define READXL_CELLLIMITS_

#include "XlsCell.h"
#include "cpp11/integers.hpp"
#include <vector>

class CellLimits {
  int minRow_, maxRow_, minCol_, maxCol_;

public:
  CellLimits() {
    minRow_ = -1;
    maxRow_ = -1;
    minCol_ = -1;
    maxCol_ = -1;
  }
  CellLimits(cpp11::integers limits) {
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

  void update(const int minRow, const int maxRow,
              const int minCol, const int maxCol) {
    minRow_ = minRow;
    maxRow_ = maxRow;
    minCol_ = minCol;
    maxCol_ = maxCol;
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
    Rprintf("row min, max: %d, %d \t col min, max: %d, %d",
            minRow_, maxRow_, minCol_, maxCol_);
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

// functions to compare a nominal cell limit (as in: "the user told me to start
// reading at row 3") to an actual cell limit (as in: "among the readable cells
// we loaded, the minimum observed row number is 4")

// What's so funny? In these variations on "less than", "greater than",
// "minimum", and "maximum", we account for the fact that, by convention, if the
// user does not specify a particular cell limit, we encode that nominal limit
// as -1.

//  1 is less than 3, in the usual sense, AND in the funny sense
// -1 is less than 3, in the usual sense, BUT NOT in the funny sense
inline bool funny_lt(const int funny, const int val) {
  return (funny >= 0) && (funny < val);
}

inline bool funny_gt(const int funny, const int val) {
  return (funny >= 0) && (funny > val);
}

inline int funny_min(const int funny, const int val) {
  return funny_lt(funny, val) ? funny : val;
}

inline int funny_max(const int funny, const int val) {
  return funny_gt(funny, val) ? funny : val;
}

template <typename T>
void insertShims(std::vector<T>& cells, const CellLimits& nominal, CellLimits& actual) {
  if (cells.empty()) {
    return;
  }

  // Recall cell limits are -1 by convention if the limit is unspecified.
  // funny_*() functions account for that.

  // if nominal min row or col is (funny) less than actual,
  // add a shim cell to the front of cells and update actual
  bool   shim_up = funny_lt(nominal.minRow(), actual.minRow());
  bool shim_left = funny_lt(nominal.minCol(), actual.minCol());
  if (shim_up || shim_left) {
    int ul_row = funny_min(nominal.minRow(), actual.minRow());
    int ul_col = funny_min(nominal.minCol(), actual.minCol());
    T ul_shim(std::make_pair(ul_row, ul_col));
    cells.insert(cells.begin(), ul_shim);
    actual.update(ul_row, ul_col);
  }

  // if nominal max row or col is (funny) greater than actual,
  // add a shim cell to the back of cells and update actual
  bool  shim_down = funny_gt(nominal.maxRow(), actual.maxRow());
  bool shim_right = funny_gt(nominal.maxCol(), actual.maxCol());
  if (shim_down || shim_right) {
    int lr_row = funny_max(nominal.maxRow(), actual.maxRow());
    int lr_col = funny_max(nominal.maxCol(), actual.maxCol());
    T lr_shim(std::make_pair(lr_row, lr_col));
    cells.push_back(lr_shim);
    actual.update(lr_row, lr_col);
  }
}

template <typename T>
typename std::vector<T>::iterator advance_row(std::vector<T>& x) {
  typename std::vector<T>::iterator it = x.begin();
  while (it != x.end() && it->row() == x.begin()->row()) {
    ++it;
  }
  return(it);
}

#endif
