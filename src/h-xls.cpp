#include <Rcpp.h>
using namespace Rcpp;

#include <libxls/xls.h>

typedef std::map<int,std::string> formatMap;

// [[Rcpp::export]]
void numSheets(std::string path) {

  xls::xlsWorkBook* pWB = xls::xls_open(path.c_str(), "UTF8");

  if (pWB == NULL)
    Rcpp::stop("Failed to open %s", path);

  Rcout << "Sheets: " << pWB->sheets.count + 1 << "\n";

  xls::xlsWorkSheet* pWS = xls_getWorkSheet(pWB, 0);
  xls_parseWorkSheet(pWS);

  int n = pWS->rows.lastrow + 1;
  int p = pWS->rows.lastcol + 1;
  Rcout << n << " x " << p << "\n";

  for (int i = 0; i < n; ++i) {
    if ((i + 1) % 10000 == 0)
      checkUserInterrupt();

    xls::st_row::st_row_data row = pWS->rows.row[i];

    for (int j = 0; j < p; ++j) {
      xls::st_cell::st_cell_data cell = row.cells.cell[j];

      // Find codes in [MS-XLS] S2.3.2 (p175).
      // See xls.c:868 for those used for cells
      switch(cell.id) {
      case 253: // LabelSst
        Rcout << cell.str << "\n";
        break;

      case 6: // formula
        // libxls doesn't record the result type of the formula, so
        // we don't support non-numeric results
        Rcout << "'" << cell.d << "'\n";
        break;

      case 189: // MulRk
      case 638: // Rk
        Rcout << cell.d << "\n";
        break;

      case 190: // MulBlank
      case 513: // Blank
        Rcout << "[BLANK]\n";
        break;

      default:
        Rcout << "Unknown type: " << cell.id << "\n";
        break;
      }
    }
  }

  // Cleanup
  xls_close(pWB);
}


// [[Rcpp::export]]
std::map<int,std::string> xls_formats(std::string path) {
  xls::xlsWorkBook* pWB = xls::xls_open(path.c_str(), "UTF8");

  if (pWB == NULL)
    Rcpp::stop("Failed to open %s", path);

  std::map<int, std::string> formats;

  int n = pWB->formats.count;
  for (int i = 0; i < n; ++i) {
    xls::st_format::st_format_data format = pWB->formats.format[i];
    std::string value((char*) pWB->formats.format[i].value);

    formats.insert(std::make_pair(format.index, value));
  }

  // Cleanup
  xls_close(pWB);

  return formats;
}

bool is_datetime(int id, formatMap formats) {
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

  formatMap::iterator format = formats.find(id);
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
