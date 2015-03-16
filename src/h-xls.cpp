#include <Rcpp.h>
using namespace Rcpp;

#include <libxls/xls.h>

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
