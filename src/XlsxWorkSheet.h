#ifndef READXL_XLSXWORKSHEET_
#define READXL_XLSXWORKSHEET_

#include "CellLimits.h"
#include "ColSpec.h"
#include "Spinner.h"
#include "XlsxCell.h"
#include "XlsxWorkBook.h"

#include "rapidxml/rapidxml.h"

#include "cpp11/integers.hpp"
#include "cpp11/list.hpp"
#include "cpp11/protect.hpp"
#include "cpp11/sexp.hpp"
#include "cpp11/strings.hpp"

// Page and section numbers below refer to
// ECMA-376 (version, date, and download URL given in XlsxCell.h)
// 18.3.1.73  row         (Row)        [p1685]
// 18.3.1.4   c           (Cell)       [p1593]
// 18.3.1.96  v           (Cell Value) [p1707]
// 18.18.11   ST_CellType (Cell Type)  [p2451]

class XlsxWorkSheet {
  // the host workbook
  XlsxWorkBook wb_;

  // xlsx specifics
  std::string sheet_;
  rapidxml::xml_document<> sheetXml_;
  rapidxml::xml_node<>* sheetData_;

  // common to xls[x]
  std::set<int> dateFormats_;
  std::vector<XlsxCell> cells_;
  std::string sheetName_;
  CellLimits nominal_, actual_;
  int ncol_, nrow_;
  Spinner spinner_;

public:

  XlsxWorkSheet(const XlsxWorkBook wb, int sheet_i,
                cpp11::integers limits, bool shim, bool progress):
  wb_(wb), nominal_(limits), spinner_(progress)
  {
    rapidxml::xml_node<>* rootNode;

    if (sheet_i >= wb.n_sheets()) {
      cpp11::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                 sheet_i + 1,  wb.n_sheets());
    }
    sheetName_ = cpp11::r_string(wb.sheets()[sheet_i]);
    std::string sheetPath = wb.sheetPath(sheet_i);
    spinner_.spin();
    sheet_ = zip_buffer(wb.path(), sheetPath);
    spinner_.spin();
    sheetXml_.parse<rapidxml::parse_strip_xml_namespaces>(&sheet_[0]);
    spinner_.spin();

    rootNode = sheetXml_.first_node("worksheet");
    if (rootNode == NULL) {
      cpp11::stop("Sheet '%s' (position %d): Invalid sheet xml (no <worksheet>)",
                  sheetName_.c_str(), sheet_i + 1);
    }

    sheetData_ = rootNode->first_node("sheetData");
    if (sheetData_ == NULL) {
      cpp11::stop("Sheet '%s' (position %d): Invalid sheet xml (no <sheetData>)",
                  sheetName_.c_str(), sheet_i + 1);
    }
    dateFormats_ = wb.dateFormats();

    // shim = TRUE when user specifies geometry via `range`
    // shim = FALSE when user specifies no geometry or uses `skip` and `n_max`
    // nominal_ holds user's geometry request, where -1 means "unspecified"
    loadCells(shim);
    // nominal_ may have been shifted (case of implicit skipping and n_max)
    // actual_ reports populated cells inside the nominal_ rectangle

    // When shim = FALSE, we shrink-wrap the data that falls inside
    // the nominal_ rectangle.
    //
    // When shim = TRUE, we may need to insert dummy cells to fill out
    // the nominal_rectangle.
    // actual_ is updated to reflect the insertions.
    if (shim) insertShims(cells_, nominal_, actual_);

    nrow_ = (actual_.minRow() < 0) ? 0 : actual_.maxRow() - actual_.minRow() + 1;
    ncol_ = (actual_.minCol() < 0) ? 0 : actual_.maxCol() - actual_.minCol() + 1;
  }

  int ncol() const {
    return ncol_;
  }

  int nrow() const {
    return nrow_;
  }

  std::string sheetName() const {
    return sheetName_;
  }

  cpp11::strings colNames(const StringSet &na, const bool trimWs) {
    cpp11::writable::strings out(ncol_);
    std::vector<XlsxCell>::iterator xcell = cells_.begin();
    int base = xcell->row();

    while(xcell != cells_.end() && xcell->row() == base) {
      xcell->inferType(na, trimWs, dateFormats_, wb_.stringTable());
      int position = (xcell->col() - actual_.minCol());
      out[position] =
        cpp11::r_string(xcell->asCharSxp(trimWs, wb_.stringTable()));
      xcell++;
    }
    return out;
  }

  std::vector<ColType> colTypes(std::vector<ColType> types,
                                const StringSet& na,
                                const bool trimWs,
                                int guess_max = 1000,
                                bool has_col_names = false) {
    if (guess_max == 0) {
      return types;
    }

    std::vector<XlsxCell>::iterator xcell;
    xcell = has_col_names ? advance_row(cells_) : cells_.begin();

    // no cell data to consult re: types
    if (xcell == cells_.end()) {
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
    int base = cells_.begin()->row() + has_col_names;
    while (xcell != cells_.end() && xcell->row() - base < guess_max) {
      count++;
      if (count % PROGRESS_TICK == 0) {
        spinner_.spin();
        cpp11::check_user_interrupt();
      }
      int j = xcell->col() - actual_.minCol();
      if (type_known[j] || types[j] == COL_TEXT) {
        xcell++;
        continue;
      }
      xcell->inferType(na, trimWs, dateFormats_, wb_.stringTable());
      ColType type = as_ColType(xcell->type());
      if (type > types[j]) {
        types[j] = type;
      }
      xcell++;
    }

    return types;
  }

  cpp11::list readCols(cpp11::strings names,
                      const std::vector<ColType>& types,
                      const StringSet& na, const bool trimWs,
                      bool has_col_names = false) {

    std::vector<XlsxCell>::iterator xcell;
    xcell = has_col_names ? advance_row(cells_): cells_.begin();

    // base is row the data starts on **in the spreadsheet**
    int base = cells_.begin()->row() + has_col_names;
    int n = (xcell == cells_.end()) ? 0 : actual_.maxRow() - base + 1;
    cpp11::writable::list cols(ncol_);
    cols.attr("names") = names;
    for (int j = 0; j < ncol_; ++j) {
      cols[j] = makeCol(types[j], n);
    }

    if (n == 0) {
      return cols;
    }

    // count is for spinner and checking for interrupt
    int count = 0;
    while (xcell != cells_.end()) {

      int i = xcell->row();
      int j = xcell->col();
      // col to write into
      int col = j - actual_.minCol();

      count++;
      if (count % PROGRESS_TICK == 0) {
        spinner_.spin();
        cpp11::check_user_interrupt();
      }

      if (types[col] == COL_SKIP) {
        xcell++;
        continue;
      }

      xcell->inferType(na, trimWs, dateFormats_, wb_.stringTable());
      CellType type = xcell->type();
      cpp11::sexp column(cols[col]);
      // row to write into
      int row = i - base;

      // Fit cell of type x into a column of type y
      // Conventions:
      //   * process type in same order as enum, unless reason to do otherwise
      //   * access cell contents only via asWhatever() methods
      switch(types[col]) {

      case COL_UNKNOWN:
      case COL_BLANK:
      case COL_SKIP:
        break;

      case COL_LOGICAL:
        if (type == CELL_DATE) {
          // print date string here, when/if it's possible to do so
          cpp11::warning("Expecting logical in %s: got a date",
                        cellPosition(i, j).c_str());
        }

        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
        case CELL_LOGICAL:
        case CELL_DATE:
        case CELL_NUMERIC:
          LOGICAL(column)[row] = xcell->asLogical();
          break;
        case CELL_TEXT: {
          std::string text_string = xcell->asStdString(trimWs, wb_.stringTable());
          bool text_boolean;
          if (logicalFromString(text_string, &text_boolean)) {
            LOGICAL(column)[row] = text_boolean;
          } else {
            cpp11::warning("Expecting logical in %s: got '%s'",
                          cellPosition(i, j).c_str(), text_string.c_str());
            LOGICAL(column)[row] = NA_LOGICAL;
          }
        }
          break;
        }
        break;

      case COL_DATE:
        if (type == CELL_LOGICAL) {
          cpp11::warning("Expecting date in %s: got boolean", cellPosition(i, j).c_str());
        }
        if (type == CELL_NUMERIC) {
          cpp11::warning("Coercing numeric to date %s", cellPosition(i, j).c_str());
        }
        if (type == CELL_TEXT) {
          cpp11::warning("Expecting date in %s: got '%s'", cellPosition(i, j).c_str(),
                         (xcell->asStdString(trimWs, wb_.stringTable())).c_str());
        }
        REAL(column)[row] = xcell->asDate(wb_.is1904());
        break;

      case COL_NUMERIC:
        if (type == CELL_LOGICAL) {
          cpp11::warning("Coercing boolean to numeric in %s", cellPosition(i, j).c_str());
        }
        if (type == CELL_DATE) {
          // print date string here, when/if possible
          cpp11::warning("Expecting numeric in %s: got a date", cellPosition(i, j).c_str());
        }
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
        case CELL_LOGICAL:
        case CELL_DATE:
        case CELL_NUMERIC:
          REAL(column)[row] = xcell->asDouble();
          break;
        case CELL_TEXT: {
          std::string num_string = xcell->asStdString(trimWs, wb_.stringTable());
          double num_num;
          bool success = doubleFromString(num_string, num_num);
          if (success) {
            cpp11::warning("Coercing text to numeric in %s: '%s'",
                          cellPosition(i, j).c_str(), num_string.c_str());
            REAL(column)[row] = num_num;
          } else {
            cpp11::warning("Expecting numeric in %s: got '%s'",
                          cellPosition(i, j).c_str(), num_string.c_str());
            REAL(column)[row] = NA_REAL;
          }
        }
          break;
        }
        break;

      case COL_TEXT:
        // not issuing warnings for NAs or coercion, because "text" is the
        // fallback column type and there are too many warnings to be helpful
        SET_STRING_ELT(column, row, xcell->asCharSxp(trimWs, wb_.stringTable()));
        break;

      case COL_LIST:
        switch(type) {
        case CELL_UNKNOWN:
        case CELL_BLANK:
          SET_VECTOR_ELT(column, row, Rf_ScalarLogical(NA_LOGICAL));
          break;
        case CELL_LOGICAL:
          SET_VECTOR_ELT(column, row, Rf_ScalarLogical(xcell->asLogical()));
          break;
        case CELL_DATE: {
          cpp11::sexp cell_val = Rf_ScalarReal(xcell->asDate(wb_.is1904()));
          cell_val.attr("class") = {"POSIXct", "POSIXt"};
          cell_val.attr("tzone") = "UTC";
          SET_VECTOR_ELT(column, row, cell_val);
          break;
        }
        case CELL_NUMERIC:
          SET_VECTOR_ELT(column, row, Rf_ScalarReal(xcell->asDouble()));
          break;
        case CELL_TEXT: {
          cpp11::writable::strings rStringVector({NA_STRING});
          SET_STRING_ELT(rStringVector, 0, xcell->asCharSxp(trimWs, wb_.stringTable()));
          SET_VECTOR_ELT(column, row, rStringVector);
          break;
        }
        }
      }
      xcell++;
    }

    return removeSkippedColumns(cols, names, types);
  }

private:

  void loadCells(const bool shim) {
    // by convention, min_row = -2 means 'read no data'
    if (nominal_.minRow() < -1) {
      return;
    }

    rapidxml::xml_node<>* row = sheetData_->first_node("row");
    if (row == NULL) {
      return;
    }

    // count is for spinner and checking for interrupt
    int count = 0;

    int i = 0;
    bool nominal_needs_checking = !shim && nominal_.maxRow() >= 0;
    for (; row; row = row->next_sibling("row")) {
      int j = 0;
      for (rapidxml::xml_node<>* cell = row->first_node("c");
           cell; cell = cell->next_sibling("c")) {
        count++;
        if (count % PROGRESS_TICK == 0) {
          spinner_.spin();
          cpp11::check_user_interrupt();
        }

        rapidxml::xml_node<>* first_child = cell->first_node(0);
        // only consider cells that have >= 1 child nodes
        // we require cell to have content, not just, e.g., a format
        if (first_child != NULL) {
          // We have a cell!

          // (i, j) is our best guess at location, but if cell declares
          // it's own location, we store that instead
          XlsxCell xcell(cell, i, j);
          i = xcell.row();
          j = xcell.col();

          if (nominal_needs_checking) {
            if (i > nominal_.minRow()) { // implicit skip happened
              nominal_.update(
                i, i + nominal_.maxRow() - nominal_.minRow(),
                nominal_.minCol(), nominal_.maxCol()
              );
            }
            nominal_needs_checking = false;
          }

          if (nominal_.contains(i, j)) {
            cells_.push_back(xcell);
            actual_.update(i, j);
          }

        }
        j++;
      }
      i++;
    }
  }

};

#endif
