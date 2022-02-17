#include "CellLimits.h"
#include "ColSpec.h"
#include "XlsWorkBook.h"
#include "XlsxWorkBook.h"
#include "XlsSheetData.h"
#include "XlsxSheetData.h"
#include "XlsCell.h"
#include "XlsxCell.h"

#include "cpp11/R.hpp"
#include "cpp11/strings.hpp"

#include <algorithm>
#include <string>
#include <vector>

class Xls {
public:
 typedef XlsWorkBook  Book;
 typedef XlsSheetData SheetData;
 typedef XlsCell      Cell;
};

class Xlsx {
public:
  typedef XlsxWorkBook  Book;
  typedef XlsxSheetData SheetData;
  typedef XlsxCell      Cell;
};

template <typename T>
class Sheet {

public:
  Sheet(const std::string& path,
        int sheet_i, cpp11::integers limits, bool shim, bool progress)
    : wb_(path),
      sd_(wb_, sheet_i, limits, shim, progress)
  {
    Rprintf("Sheet() constructor\n");
    Rprintf("Reading from: %s\n", wb_.path().c_str());
    Rprintf(
      "Reading %d rows x %d cols from worksheet '%s'\n",
      sd_.nrow(), sd_.ncol(), sd_.sheetName().c_str());
  }

  cpp11::strings colNames(const StringSet &na, const bool trimWs) {
    cpp11::writable::strings out(sd_.ncol());
    typename std::vector<typename T::Cell>::iterator xcell = sd_.cells_.begin();
    int base = xcell->row();

    while(xcell != sd_.cells_.end() && xcell->row() == base) {
      xcell->inferType(na, trimWs, wb_.dateFormats(), wb_.stringTable());
      int position = xcell->col() - sd_.startCol();
      out[position] = cpp11::r_string(xcell->asCharSxp(trimWs, wb_.stringTable()));
      xcell++;
    }
    return out;
  }

  std::vector<ColType> colTypes(std::vector<ColType> types,
                                const StringSet &na,
                                const bool trimWs,
                                int guess_max = 1000,
                                bool has_col_names = false) {
    if (guess_max == 0) {
      return types;
    }

    typename std::vector<typename T::Cell>::iterator xcell;
    xcell = has_col_names ? advance_row(sd_.cells_) : sd_.cells_.begin();

    // no cell data to consult re: types
    if (xcell == sd_.cells_.end()) {
      std::fill(types.begin(), types.end(), COL_BLANK);
      return types;
    }

    std::vector<bool> type_known(types.size());
    for (size_t j = 0; j < types.size(); j++) {
      type_known[j] = types[j] != COL_UNKNOWN;
    }

    // count is for spinner and checking for interrupt
    int count = 0;
    // base is row the data starts on **in the spreadsheet**
    int base = sd_.cells_.begin()->row() + has_col_names;
    while (xcell != sd_.cells_.end() && xcell->row() - base < guess_max) {
      count++;
      if (count % PROGRESS_TICK == 0) {
        //spinner_.spin();
        cpp11::check_user_interrupt();
      }
      int j = xcell->col() - sd_.startCol();
      if (type_known[j] || types[j] == COL_TEXT) {
        xcell++;
        continue;
      }
      xcell->inferType(na, trimWs, wb_.dateFormats(), wb_.stringTable());
      ColType type = as_ColType(xcell->type());
      if (type > types[j]) {
        types[j] = type;
      }
      xcell++;
    }

    return types;
  }

  int ncol() const { return sd_.ncol(); }
  int nrow() const { return sd_.nrow(); }

private:
  typename T::Book wb_;
  typename T::SheetData sd_;
};
