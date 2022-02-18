#pragma once

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

class XlsxCellSet {
  // xlsx specifics

  // http://rapidxml.sourceforge.net/manual.html
  // "In-situ parsing requires that source text lives at least as long as the
  // document object. If source text is destroyed, names and values of nodes in
  // DOM tree will become destroyed as well."
  // sheetXml_ = the document object
  // sheet_    = the source text
  // I.e. sheet_ is the memory that backs sheetXml_ and therefore must belong
  // to the class and cannot just be local to the constructor.
  rapidxml::xml_document<> sheetXml_;
  std::string sheet_;
  rapidxml::xml_node<>* sheetData_;

  // common to xls[x]
  Spinner spinner_;
  std::string sheetName_;
  CellLimits nominal_, actual_;
  int ncol_, nrow_;

public:

  std::vector<XlsxCell> cells_;

  XlsxCellSet(const XlsxWorkBook wb, int sheet_i,
                cpp11::integers limits, bool shim, bool progress)
    :  spinner_(progress), nominal_(limits)
  {
    //Rprintf("XlsxCellSet() constructor\n");

    rapidxml::xml_node<>* rootNode;

    if (sheet_i >= wb.n_sheets()) {
      cpp11::stop("Can't retrieve sheet in position %d, only %d sheet(s) found.",
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
    if (!rootNode) {
      cpp11::stop("Sheet '%s' (position %d): Invalid sheet xml (no <worksheet>)",
                  sheetName_.c_str(), sheet_i + 1);
    }

    sheetData_ = rootNode->first_node("sheetData");
    if (!sheetData_) {
      cpp11::stop("Sheet '%s' (position %d): Invalid sheet xml (no <sheetData>)",
                  sheetName_.c_str(), sheet_i + 1);
    }

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

  int ncol() const { return ncol_; }
  int nrow() const { return nrow_; }
  std::string sheetName() const { return sheetName_; }
  int startCol() const { return actual_.minCol(); }
  int lastRow() const { return actual_.maxRow(); }

private:

  void loadCells(const bool shim) {
    // by convention, min_row = -2 means 'read no data'
    if (nominal_.minRow() < -1) {
      return;
    }

    rapidxml::xml_node<>* row = sheetData_->first_node("row");
    if (!row) {
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
        if (first_child) {
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
