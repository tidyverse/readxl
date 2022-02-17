#include "XlsWorkBook.h"
#include "XlsxWorkBook.h"
#include "XlsSheetData.h"
#include "XlsxSheetData.h"
#include "XlsCell.h"
#include "XlsxCell.h"

#include "cpp11/R.hpp"
#include "cpp11/strings.hpp"

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

  int ncol() const { return sd_.ncol(); }
  int nrow() const { return sd_.nrow(); }

private:
  typename T::Book wb_;
  typename T::SheetData sd_;
};
