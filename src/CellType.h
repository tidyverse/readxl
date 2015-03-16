#ifndef EXELL_CELLTYPE_
#define EXELL_CELLTYPE_

#include <Rcpp.h>
#include <libxls/xls.h>
#include "XlsWorkBook.h"

enum CellType {
  CELL_BLANK,
  CELL_DATE,
  CELL_NUMERIC,
  CELL_TEXT
};

inline CellType cellType(xls::st_cell::st_cell_data cell) {
  // Find codes in [MS-XLS] S2.3.2 (p175).
  // See xls_addCell for those used for cells
  switch(cell.id) {
  case 253: // LabelSst
  case 516: // Label
    return CELL_TEXT;
    break;

  case 6:    // formula
  case 1030: // formula (Apple Numbers Bug)
    if (cell.l == 0) {
      return CELL_NUMERIC;
    } else {
      return CELL_TEXT;
    }
    break;

  case 189: // MulRk
  case 515: // Number
  case 638: // Rk
    return CELL_NUMERIC;
    break;

  case 190: // MulBlank
  case 513: // Blank
    return CELL_BLANK;
    break;

  default:
    Rcout << "Unknown type: " << cell.id << "\n";
  return CELL_NUMERIC;
  }
}

inline bool isBlank(xls::st_cell::st_cell_data cell)  {
  return cell.id == 190 || cell.id == 513;
}

bool inline is_datetime(int id, const FormatMap& formats) {
  // Date formats:
  // ECMA-376 (http://www.ecma-international.org/publications/standards/Ecma-376.htm)
  // 18.8.30 numFmt (Number Format)  (p1777)
  // Date times: 14-22, 27-36, 45-47, 50-58, 71-81 (inclusive)
  if ((id >= 14 & id <= 22) ||
    (id >= 27 & id <= 36) ||
    (id >= 45 & id <= 47) ||
    (id >= 50 & id <= 58) ||
    (id >= 71 & id <= 81))
    return true;

  // Built-in format that's not a date
  if (id < 164)
    return false;

  if (id > 382)
    Rcpp::stop("Invalid format specifier (%i)", id);

  FormatMap::const_iterator format = formats.find(id);
  if (format == formats.end())
    Rcpp::stop("Customer format specifier not defined (%i)", id);

  std::string formatString = format->second;
  for (int i = 0; i < formatString.size(); ++i) {
    switch (formatString[i]) {
    case 'd':
    case 'm': // 'mm' for minutes
    case 'y':
    case 'h': // 'hh'
    case 's': // 'ss'
      return true;
    default:
      break;
    }
  }
  return false;
}

#endif
