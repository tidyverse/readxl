#include <sys/time.h> // alpine linux / musl must be before others
#include <unistd.h>   // alpine linux / musl must be before others

#include "CellLimits.h"
#include "ColSpec.h"
#include "XlsWorkBook.h"
#include "XlsxWorkBook.h"
#include "XlsCellSet.h"
#include "XlsxCellSet.h"
#include "XlsCell.h"
#include "XlsxCell.h"

#include "cpp11/R.hpp"
#include "cpp11/strings.hpp"

#include <algorithm>
#include <string>
#include <vector>

class Xls {
public:
 typedef XlsWorkBook Book;
 typedef XlsCellSet  CellSet;
 typedef XlsCell     Cell;
};

class Xlsx {
public:
  typedef XlsxWorkBook Book;
  typedef XlsxCellSet  CellSet;
  typedef XlsxCell     Cell;
};

template <typename T>
class SheetView {

  Spinner spinner_;
  typename T::Book wb_;
  typename T::CellSet cs_;

public:
  SheetView(const std::string& path,
            int sheet_i, cpp11::integers limits, bool shim, bool progress, bool extract_colors = false)
    : spinner_(progress),
      wb_(path),
      cs_(wb_, sheet_i, limits, shim, spinner_, extract_colors)
  {
  }

  int ncol() const { return cs_.ncol(); }
  int nrow() const { return cs_.nrow(); }

  cpp11::strings colNames(const StringSet &na, const bool trimWs) {
    cpp11::writable::strings out(cs_.ncol());
    typename std::vector<typename T::Cell>::iterator xcell = cs_.cells_.begin();
    int base = xcell->row();

    while(xcell != cs_.cells_.end() && xcell->row() == base) {
      xcell->inferType(na, trimWs, wb_.dateFormats(), wb_.stringTable());
      int position = xcell->col() - cs_.startCol();
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
    xcell = has_col_names ? advance_row(cs_.cells_) : cs_.cells_.begin();

    // no cell data to consult re: types
    if (xcell == cs_.cells_.end()) {
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
    int base = cs_.cells_.begin()->row() + has_col_names;
    while (xcell != cs_.cells_.end() && xcell->row() - base < guess_max) {
      count++;
      if (count % PROGRESS_TICK == 0) {
        spinner_.spin();
        cpp11::check_user_interrupt();
      }
      int j = xcell->col() - cs_.startCol();
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

  cpp11::list readCols(cpp11::strings names,
                       const std::vector<ColType>& types,
                       const StringSet &na, const bool trimWs,
                       bool has_col_names = false) {

    typename std::vector<typename T::Cell>::iterator xcell;
    xcell = has_col_names ? advance_row(cs_.cells_) : cs_.cells_.begin();

    // base is row the data starts on **in the spreadsheet**
    int base = cs_.cells_.begin()->row() + has_col_names;
    int n = (xcell == cs_.cells_.end()) ? 0 : cs_.lastRow() - base + 1;
    
    // First pass: determine which columns have background colors if extract_colors is enabled
    std::vector<bool> has_colors(cs_.ncol(), false);
    int color_column_count = 0;
    
    if (cs_.extract_colors_ && n > 0) {
      typename std::vector<typename T::Cell>::iterator scan_cell = xcell;
      while (scan_cell != cs_.cells_.end()) {
        int col = scan_cell->col() - cs_.startCol();
        if (col >= 0 && col < cs_.ncol() && types[col] != COL_SKIP) {
          // Check static background color
          std::string bg_color = scan_cell->getBackgroundColor(wb_.backgroundColors());
          // Check conditional formatting color (for XLSX)
          std::string cf_color = getConditionalFormattingColor(*scan_cell);
          
          if (!bg_color.empty() || !cf_color.empty()) {
            if (!has_colors[col]) {
              has_colors[col] = true;
              color_column_count++;
            }
          }
        }
        scan_cell++;
      }
    }
    
    // Determine the number of columns - add only color columns that have actual colors
    int total_cols = cs_.ncol() + color_column_count;
    cpp11::writable::list cols(total_cols);
    
    // Create column names - add "_bg" suffix only for columns with colors
    cpp11::writable::strings col_names(total_cols);
    std::vector<int> color_col_mapping(cs_.ncol(), -1); // maps data col to color col index
    int next_color_col = cs_.ncol();
    
    for (int j = 0; j < cs_.ncol(); ++j) {
      col_names[j] = names[j];
      if (cs_.extract_colors_ && has_colors[j]) {
        std::string base_name = static_cast<std::string>(names[j]);
        std::string color_name = base_name + "_bg";
        col_names[next_color_col] = color_name;
        color_col_mapping[j] = next_color_col;
        next_color_col++;
      }
    }
    cols.attr("names") = col_names;
    
    // Create data columns
    for (int j = 0; j < cs_.ncol(); ++j) {
      cols[j] = makeCol(types[j], n);
    }
    // Create color columns only for columns that have colors
    for (int j = 0; j < cs_.ncol(); ++j) {
      if (cs_.extract_colors_ && has_colors[j]) {
        cols[color_col_mapping[j]] = makeCol(COL_TEXT, n);
      }
    }

    if (n == 0) {
      return cols;
    }

    // count is for spinner and checking for interrupt
    int count = 0;
    while (xcell != cs_.cells_.end()) {

      int i = xcell->row();
      int j = xcell->col();
      // col to write into
      int col = j - cs_.startCol();

      count++;
      if (count % PROGRESS_TICK == 0) {
        spinner_.spin();
        cpp11::check_user_interrupt();
      }

      if (types[col] == COL_SKIP) {
        xcell++;
        continue;
      }

      xcell->inferType(na, trimWs, wb_.dateFormats(), wb_.stringTable());
      CellType type = xcell->type();
      cpp11::sexp column = cpp11::as_sexp(cols[col]);
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
          cpp11::warning("Coercing numeric to date in %s",
                         cellPosition(i, j).c_str());
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
      
      // Extract background color if enabled and this column has colors
      if (cs_.extract_colors_ && types[col] != COL_SKIP && color_col_mapping[col] != -1) {
        cpp11::sexp color_column = cpp11::as_sexp(cols[color_col_mapping[col]]);
        std::string bg_color = xcell->getBackgroundColor(wb_.backgroundColors());
        
        // If no static background color, check conditional formatting
        if (bg_color.empty()) {
          bg_color = getConditionalFormattingColor(*xcell);
        }
        
        if (bg_color.empty()) {
          SET_STRING_ELT(color_column, row, NA_STRING);
        } else {
          SET_STRING_ELT(color_column, row, Rf_mkCharCE(bg_color.c_str(), CE_UTF8));
        }
      }
      
      xcell++;
    }

    // Handle column filtering for color extraction
    if (cs_.extract_colors_ && color_column_count > 0) {
      // Create extended types vector for both data and color columns
      std::vector<ColType> extended_types(total_cols);
      cpp11::writable::strings extended_names(total_cols);
      
      for (int j = 0; j < cs_.ncol(); ++j) {
        extended_types[j] = types[j];
        extended_names[j] = names[j];
      }
      
      // Add types for color columns (they follow same skip pattern as their data columns)
      int color_idx = cs_.ncol();
      for (int j = 0; j < cs_.ncol(); ++j) {
        if (has_colors[j]) {
          extended_types[color_idx] = types[j];
          extended_names[color_idx] = col_names[color_idx];
          color_idx++;
        }
      }
      
      return removeSkippedColumns(cols, extended_names, extended_types);
    } else {
      return removeSkippedColumns(cols, names, types);
    }
  }

private:
  // Get conditional formatting color - non-template method
  std::string getConditionalFormattingColor(const typename T::Cell& cell) {
    return ""; // Default: no conditional formatting for XLS
  }

};

// Template specialization for XLSX conditional formatting
template<>
inline std::string SheetView<Xlsx>::getConditionalFormattingColor(const XlsxCell& cell) {
  return cell.getConditionalFormattingColor(cs_.conditionalFormats());
}
