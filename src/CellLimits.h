#ifndef READXL_CELLLIMITS_
#define READXL_CELLLIMITS_

#include <Rcpp.h>
#include "XlsCell.h"

class CellLimits {
  std::map<std::string,int> limits_;

public:
  CellLimits(const int minRow = -1,
             const int maxRow = -1,
             const int minCol = -1,
             const int maxCol = -1) {
    limits_["min_row"] = minRow;
    limits_["max_row"] = maxRow;
    limits_["min_col"] = minCol;
    limits_["max_col"] = maxCol;
  }

  CellLimits(Rcpp::IntegerVector limits) {
    limits_["min_row"] = limits[0];
    limits_["max_row"] = limits[1];
    limits_["min_col"] = limits[2];
    limits_["max_col"] = limits[3];
  }

  int min_row() const {
    return limits_.find("min_row")->second;
  }
  int max_row() const {
    return limits_.find("max_row")->second;
  }
  int min_col() const {
    return limits_.find("min_col")->second;
  }
  int max_col() const {
    return limits_.find("max_col")->second;
  }

  void update(const XlsCell cell) {
    update(cell.row(), cell.col());
  }

  void update(const int row, const int col) {
    if (this->min_row() < 0 || row < this->min_row()) {
      limits_["min_row"] = row;
    }
    if (row > this->max_row()) {
      limits_["max_row"] = row;
    }
    if (this->min_col() < 0 || col < this->min_col()) {
      limits_["min_col"] = col;
    }
    if (col > this->max_col()) {
      limits_["max_col"] = col;
    }
  }

  bool contains(const XlsCell cell) const {
    return contains(this->min_row(), this->max_row(), cell.row()) &&
      contains(this->min_col(), this->max_col(), cell.col());
  }

  void print() {
    Rcpp::Rcout << "row min, max: " << this->min_row() << ", "
                << this->max_row() << "\t"
                << "col min, max: " << this->min_col() << ", "
                << this->max_col() << std::endl;
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
