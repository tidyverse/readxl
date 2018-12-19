#ifndef READXL_XLSXWORKSHEET_
#define READXL_XLSXWORKSHEET_

#include <Rcpp.h>
#include "rapidxml.h"
#include "XlsxWorkBook.h"
#include "Spinner.h"
#include "XlsxCell.h"
#include "ColSpec.h"
#include "CellLimits.h"

const int PROGRESS_TICK = 131072; // 2^17

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
                Rcpp::IntegerVector limits, bool shim, bool progress):
  wb_(wb), nominal_(limits), spinner_(progress)
  {
    rapidxml::xml_node<>* rootNode;

    if (sheet_i >= wb.n_sheets()) {
      Rcpp::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
                 sheet_i + 1,  wb.n_sheets());
    }
    sheetName_ = wb.sheets()[sheet_i];
    std::string sheetPath = wb.sheetPath(sheet_i);
    spinner_.spin();
    sheet_ = zip_buffer(wb.path(), sheetPath);
    spinner_.spin();
    sheetXml_.parse<rapidxml::parse_strip_xml_namespaces>(&sheet_[0]);
    spinner_.spin();

    rootNode = sheetXml_.first_node("worksheet");
    if (rootNode == NULL) {
      Rcpp::stop("Sheet '%s' (position %d): Invalid sheet xml (no <worksheet>)",
                 sheetName_, sheet_i + 1);
    }

    sheetData_ = rootNode->first_node("sheetData");
    if (sheetData_ == NULL) {
      Rcpp::stop("Sheet '%s' (position %d): Invalid sheet xml (no <sheetData>)",
                 sheetName_, sheet_i + 1);
    }
    dateFormats_ = wb.dateFormats();

    // nominal_ holds user's geometry request
    loadCells(shim);
    // nominal_ may have been shifted (case of implicit skipping and n_max)
    // actual_ reports populated cells inside the nominal_ rectangle

    // insert shims and update actual_
    if (shim) insertShims();

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

  Rcpp::CharacterVector colNames(const StringSet &na, const bool trimWs) {
    Rcpp::CharacterVector out(ncol_);
    std::vector<XlsxCell>::iterator xcell = cells_.begin();
    int base = xcell->row();

    while(xcell != cells_.end() && xcell->row() == base) {
      xcell->inferType(na, trimWs, wb_.stringTable(), dateFormats_);
      out[xcell->col() - actual_.minCol()] =
        xcell->asCharSxp(wb_.stringTable(), trimWs);
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
        Rcpp::checkUserInterrupt();
      }
      int j = xcell->col() - actual_.minCol();
      if (type_known[j] || types[j] == COL_TEXT) {
        xcell++;
        continue;
      }
      xcell->inferType(na, trimWs, wb_.stringTable(), dateFormats_);
      ColType type = as_ColType(xcell->type());
      if (type > types[j]) {
        types[j] = type;
      }
      xcell++;
    }

    return types;
  }

  Rcpp::List readCols(Rcpp::CharacterVector names,
                      const std::vector<ColType>& types,
                      const StringSet& na, const bool trimWs,
                      bool has_col_names = false) {

    std::vector<XlsxCell>::iterator xcell;
    xcell = has_col_names ? advance_row(cells_): cells_.begin();

    // base is row the data starts on **in the spreadsheet**
    int base = cells_.begin()->row() + has_col_names;
    int n = (xcell == cells_.end()) ? 0 : actual_.maxRow() - base + 1;
    Rcpp::List cols(ncol_);
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
        Rcpp::checkUserInterrupt();
      }

      if (types[col] == COL_SKIP) {
        xcell++;
        continue;
      }

      xcell->inferType(na, trimWs, wb_.stringTable(), dateFormats_);
      CellType type = xcell->type();
      Rcpp::RObject column = cols[col];
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
          Rcpp::warning("Expecting logical in %s: got a date",
                        cellPosition(i, j));
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
          std::string text_string = xcell->asStdString(wb_.stringTable(), trimWs);
          bool text_boolean;
          if (logicalFromString(text_string, &text_boolean)) {
            LOGICAL(column)[row] = text_boolean;
          } else {
            Rcpp::warning("Expecting logical in %s: got '%s'",
                          cellPosition(i, j), text_string);
            LOGICAL(column)[row] = NA_LOGICAL;
          }
        }
          break;
        }
        break;

      case COL_DATE:
        if (type == CELL_LOGICAL) {
          Rcpp::warning("Expecting date in %s: got boolean", cellPosition(i, j));
        }
        if (type == CELL_NUMERIC) {
          Rcpp::warning("Coercing numeric to date %s", cellPosition(i, j));
        }
        if (type == CELL_TEXT) {
          Rcpp::warning("Expecting date in %s: got '%s'", cellPosition(i, j),
                        xcell->asStdString(wb_.stringTable(), trimWs));
        }
        REAL(column)[row] = xcell->asDate(wb_.is1904());
        break;

      case COL_NUMERIC:
        if (type == CELL_LOGICAL) {
          Rcpp::warning("Coercing boolean to numeric in %s", cellPosition(i, j));
        }
        if (type == CELL_DATE) {
          // print date string here, when/if possible
          Rcpp::warning("Expecting numeric in %s: got a date", cellPosition(i, j));
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
          std::string num_string = xcell->asStdString(wb_.stringTable(), trimWs);
          double num_num;
          bool success = doubleFromString(num_string, num_num);
          if (success) {
            Rcpp::warning("Coercing text to numeric in %s: '%s'",
                          cellPosition(i, j), num_string);
            REAL(column)[row] = num_num;
          } else {
            Rcpp::warning("Expecting numeric in %s: got '%s'",
                          cellPosition(i, j), num_string);
            REAL(column)[row] = NA_REAL;
          }
        }
          break;
        }
        break;

      case COL_TEXT:
        // not issuing warnings for NAs or coercion, because "text" is the
        // fallback column type and there are too many warnings to be helpful
        SET_STRING_ELT(column, row, xcell->asCharSxp(wb_.stringTable(), trimWs));
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
          Rcpp::RObject cell_val = Rf_ScalarReal(xcell->asDate(wb_.is1904()));
          cell_val.attr("class") = Rcpp::CharacterVector::create("POSIXct", "POSIXt");
          cell_val.attr("tzone") = "UTC";
          SET_VECTOR_ELT(column, row, cell_val);
          break;
        }
        case CELL_NUMERIC:
          SET_VECTOR_ELT(column, row, Rf_ScalarReal(xcell->asDouble()));
          break;
        case CELL_TEXT: {
          Rcpp::CharacterVector rStringVector = Rcpp::CharacterVector(1, NA_STRING);
          SET_STRING_ELT(rStringVector, 0, xcell->asCharSxp(wb_.stringTable(), trimWs));
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
          Rcpp::checkUserInterrupt();
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

  // shim = TRUE when user specifies geometry via `range`
  // shim = FALSE when user specifies no geometry or uses `skip` and `n_max`
  //
  // nominal_ reflects user's geometry request
  // actual_ reports populated cells inside the nominal_ rectangle
  //
  // When shim = FALSE, we shrink-wrap the data that falls inside
  // the nominal_ rectangle.
  //
  // When shim = TRUE, we may need to insert dummy cells to fill out
  // the nominal_rectangle.
  //
  // actual_ is updated to reflect the insertions
  void insertShims() {

    // no cells were loaded
    if (cells_.empty()) {
      return;
    }

    // Recall cell limits are -1 by convention if the limit is unspecified.
    // funny_*() functions account for that.

    // if nominal min row or col is less than actual,
    // add a shim cell to the front of cells_
    bool   shim_up = funny_lt(nominal_.minRow(), actual_.minRow());
    bool shim_left = funny_lt(nominal_.minCol(), actual_.minCol());
    if (shim_up || shim_left) {
      int ul_row = funny_min(nominal_.minRow(), actual_.minRow());
      int ul_col = funny_min(nominal_.minCol(), actual_.minCol());
      XlsxCell ul_shim(std::make_pair(ul_row, ul_col));
      cells_.insert(cells_.begin(), ul_shim);
      actual_.update(ul_row, ul_col);
    }

    // if nominal max row or col is greater than actual,
    // add a shim cell to the back of cells_
    bool  shim_down = funny_gt(nominal_.maxRow(), actual_.maxRow());
    bool shim_right = funny_gt(nominal_.maxCol(), actual_.maxCol());
    if (shim_down || shim_right) {
      int lr_row = funny_max(nominal_.maxRow(), actual_.maxRow());
      int lr_col = funny_max(nominal_.maxCol(), actual_.maxCol());
      XlsxCell lr_shim(std::make_pair(lr_row, lr_col));
      cells_.push_back(lr_shim);
      actual_.update(lr_row, lr_col);
    }
  }

  bool funny_lt(const int funny, const int val) {
    return (funny >= 0) && (funny < val);
  }

  bool funny_gt(const int funny, const int val) {
    return (funny >= 0) && (funny > val);
  }

  int funny_min(const int funny, const int val) {
    return funny_lt(funny, val) ? funny : val;
  }

  int funny_max(const int funny, const int val) {
    return funny_gt(funny, val) ? funny : val;
  }

  std::vector<XlsxCell>::iterator advance_row(std::vector<XlsxCell>& x) {
    std::vector<XlsxCell>::iterator it = x.begin();
    while (it != x.end() && it->row() == x.begin()->row()) {
      ++it;
    }
    return(it);
  }

};

#endif
