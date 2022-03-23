#pragma once

#include "utils.h"
#include "StringSet.h"

#include "libxls/xls.h"

#include "cpp11/doubles.hpp"
#include "cpp11/list.hpp"
#include "cpp11/logicals.hpp"
#include "cpp11/protect.hpp"
#include "cpp11/sexp.hpp"
#include "cpp11/strings.hpp"

enum CellType {
  CELL_UNKNOWN,
  CELL_BLANK,
  CELL_LOGICAL,
  CELL_DATE,
  CELL_NUMERIC,
  CELL_TEXT
};

enum ColType {
  COL_UNKNOWN, // implies column type needs to be guessed
  COL_BLANK,   // occurs when col_types = NULL and observe only CELL_BLANKs
  COL_LOGICAL,
  COL_DATE,
  COL_NUMERIC,
  COL_TEXT,
  COL_LIST,    // occurs only as user-specified column type
  COL_SKIP     // occurs only as user-specified column type
};

// ColType enum is ordered such that a simple cast is sufficient to convert
// from CellType (user-specified column types come after cell types)
ColType inline as_ColType(CellType celltype) {
  return (ColType) celltype;
}

inline std::string cellTypeDesc(CellType type) {
  switch(type) {
  case CELL_UNKNOWN:  return "unknown";
  case CELL_BLANK:    return "blank";
  case CELL_LOGICAL:  return "logical";
  case CELL_DATE:     return "date";
  case CELL_NUMERIC:  return "numeric";
  case CELL_TEXT:     return "text";
  }
  return "???";
}

inline std::string colTypeDesc(ColType type) {
  switch(type) {
  case COL_UNKNOWN: return "unknown";
  case COL_BLANK:   return "blank";
  case COL_LOGICAL: return "logical";
  case COL_DATE:    return "date";
  case COL_NUMERIC: return "numeric";
  case COL_TEXT:    return "text";
  case COL_LIST:    return "list";
  case COL_SKIP:    return "skip";
  }
  return "???";
}

inline cpp11::strings colTypeDescs(std::vector<ColType> types) {
  cpp11::writable::strings out(types.size());
  for (size_t i = 0; i < types.size(); ++i) {
    out[i] = colTypeDesc(types[i]);
  }
  return out;
}

inline std::vector<ColType> colTypeStrings(cpp11::strings x) {
  std::vector<ColType> types;
  types.reserve(x.size());

  for (int i = 0; i < x.size(); ++i) {
    std::string type(x[i]);
    if (type == "guess") {
      types.push_back(COL_UNKNOWN);
    } else if (type == "blank") {
      types.push_back(COL_BLANK);
    } else if (type == "logical") {
      types.push_back(COL_LOGICAL);
    } else if (type == "date") {
      types.push_back(COL_DATE);
    } else if (type == "numeric") {
      types.push_back(COL_NUMERIC);
    } else if (type == "text") {
      types.push_back(COL_TEXT);
    } else if (type == "list") {
      types.push_back(COL_LIST);
    } else if (type == "skip") {
      types.push_back(COL_SKIP);
    } else {
      cpp11::stop("Unknown column type '%s' at position %i", type.c_str(), (i + 1));
    }
  }

  return types;
}

bool inline requiresGuess(std::vector<ColType> types) {
  std::vector<ColType>::iterator iter;
  iter = find(types.begin(), types.end(), COL_UNKNOWN);
  return iter != types.end();
}

bool inline isDateTime(int id, const std::set<int> custom) {
  // Page and section numbers below refer to
  // ECMA-376 (version, date, and download URL given in XlsxCell.h)
  //
  // Example from L.2.7.4.4 p4698 for hypothetical cell D2
  // Cell D2 contains the text "Q1" and is defined in the cell table of sheet1
  // as:
  //
  // <c r="D2" s="7" t="s">
  //     <v>0</v>
  // </c>
  //
  // On this cell, the attribute value s="7" indicates that the 7th (zero-based)
  // <xf> definition of <cellXfs> holds the formatting information for the cell.
  // The 7th <xf> of <cellXfs> is defined as:
  //
  // <xf numFmtId="0" fontId="4" fillId="2" borderId="2" xfId="1" applyBorder="1"/>
  //
  // The number formatting information cannot be found in a <numFmt> definition
  // because it is a built-in format; instead, it is implicitly understood to be
  // the 0th built-in number format.
  //
  // This function stores knowledge about these built-in number formats.
  //
  // 18.8.30 numFmt (Number Format) p1786
  // Date times: 14-22, 27-36, 45-47, 50-58, 71-81 (inclusive)
  if ((id >= 14 && id <= 22) ||
      (id >= 27 && id <= 36) ||
      (id >= 45 && id <= 47) ||
      (id >= 50 && id <= 58) ||
      (id >= 71 && id <= 81))
    return true;

  // Built-in format that's not a date
  if (id < 164)
    return false;

  return custom.count(id) > 0;
}

inline bool isDateFormat(std::string x) {
  // TO FIX? So far no bug reports due to this.
  // Logic below is too simple. For example, it deems this format string a date:
  // "$"#,##0_);[Red]\("$"#,##0\)
  // because of the `d` in `[Red]`
  //
  // Ideally this can wait until we are using something like
  // https://github.com/WizardMac/TimeFormatStrings
  // which presumably offers fancier ways to analyze format codes.
  for (size_t i = 0; i < x.size(); ++i) {
    switch (x[i]) {
    case 'd':
    case 'D':
    case 'm': // 'mm' for minutes
    case 'M':
    case 'y':
    case 'Y':
    case 'h': // 'hh'
    case 'H':
    case 's': // 'ss'
    case 'S':
      return true;
    default:
      break;
    }
  }

  return false;
}

// Adapted from @reviewher https://github.com/tidyverse/readxl/issues/388
// See also ECMA Part 1 page 1785 (actual page 1795) section 18.8.31 "numFmts
// (Number Formats)"
#define CASEI(c) case c: case (c | 0x20)
#define CMPLC(j,n) if(x[i+j] | (0x20 == n))
inline bool isDateFormat2(std::string x) {
  char escaped = 0;
  char bracket = 0;
  for (size_t i = 0; i < x.size(); ++i) switch (x[i]) {
    CASEI('D'):
    // https://github.com/nacnudus/tidyxl/pull/75
    // CASEI('E'):
    CASEI('H'):
    CASEI('M'):
    CASEI('S'):
    CASEI('Y'):
      if(!escaped && !bracket) return true;
      break;
    case '"':
      escaped = 1 - escaped; break;
    case '\\':
    case '_':
      ++i;
      break;
    case '[': if(!escaped) bracket = 1; break;
    case ']': if(!escaped) bracket = 0; break;
    CASEI('G'):
      if(i + 6 < x.size())
      CMPLC(1,'e')
      CMPLC(2,'n')
      CMPLC(3,'e')
      CMPLC(4,'r')
      CMPLC(5,'a')
      CMPLC(6,'l')
        return false;
  }
  return false;
}
#undef CMPLC
#undef CASEI

inline std::vector<ColType> recycleTypes(std::vector<ColType> types,
                                         int ncol) {
  if (types.size() == 1) {
    types.resize(ncol);
    std::fill(types.begin(), types.end(), types[0]);
  }
  return types;
}

inline std::vector<ColType> finalizeTypes(std::vector<ColType> types) {
  // convert blank columns to a default type: logical
  // can only happen when col_type = guess AND
  //   * all cells in column are empty or
  //   * all cells match one of the na strings or
  //   * there is no cell data at all
  for (size_t i = 0; i < types.size(); i++) {
    if (types[i] == COL_BLANK || types[i] == COL_UNKNOWN) {
      types[i] = COL_LOGICAL;
    }
  }
  return types;
}

inline cpp11::strings reconcileNames(cpp11::strings names,
                                            const std::vector<ColType>& types,
                                            int sheet_i) {
  size_t ncol_names = names.size();
  size_t ncol_types = types.size();

  if (ncol_names == ncol_types) {
    return names;
  }

  size_t ncol_noskip = 0;
  for (size_t i = 0; i < types.size(); i++) {
    if (types[i] != COL_SKIP) {
      ncol_noskip++;
    }
  }
  if (ncol_names != ncol_noskip) {
    cpp11::stop("Sheet %d has %d columns (%d unskipped), but `col_names` has length %d.",
               (sheet_i + 1), ncol_types, ncol_noskip, ncol_names);
  }

  cpp11::writable::strings newNames(ncol_types);
  size_t j_short = 0;
  for (size_t j_long = 0; j_long < ncol_types; ++j_long) {
    if (types[j_long] == COL_SKIP) {
      continue;
    }
    newNames[j_long] = names[j_short];
    j_short++;
  }
  return newNames;
}

inline cpp11::sexp makeCol(ColType type, int n) {
  switch(type) {
  case COL_UNKNOWN:
  case COL_BLANK:
  case COL_SKIP:
    return R_NilValue;
  case COL_LOGICAL:
    return new_vector<cpp11::writable::logicals>(n, NA_LOGICAL);
  case COL_DATE: {
    cpp11::sexp col = new_vector<cpp11::writable::doubles>(n, NA_REAL);
    col.attr("class") = {"POSIXct", "POSIXt"};
    col.attr("tzone") = "UTC";
    return col;
  }
  case COL_NUMERIC:
    return new_vector<cpp11::writable::doubles>(n, NA_REAL);
  case COL_TEXT:
    return new_vector<cpp11::writable::strings>(n, NA_STRING);
  case COL_LIST:
    return new_vector<cpp11::writable::list>(n, new_vector<cpp11::writable::logicals>(1, NA_LOGICAL));
  }

  return R_NilValue;
}

inline cpp11::list removeSkippedColumns(cpp11::list cols,
                                       cpp11::strings names,
                                       std::vector<ColType> types) {
  int p = cols.size();

  int p_out = 0;
  for (int j = 0; j < p; ++j) {
    if (types[j] != COL_SKIP)
      p_out++;
  }

  cpp11::writable::list out(p_out);
  cpp11::writable::strings names_out(p_out);
  int j_out = 0;
  for (int j = 0; j < p; ++j) {
    if (types[j] == COL_SKIP) {
      continue;
    }

    out[j_out] = cols[j];
    names_out[j_out] = names[j];
    j_out++;
  }

  // Turn list into a data frame
  out.attr("names") = names_out;

  return out;
}
