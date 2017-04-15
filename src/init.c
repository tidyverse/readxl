#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* FIXME:
Check these declarations against the C/Fortran source code.
*/

/* .Call calls */
extern SEXP readxl_parse_ref(SEXP);
extern SEXP readxl_read_xls_(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP readxl_read_xlsx_(SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP, SEXP);
extern SEXP readxl_xls_date_formats(SEXP);
extern SEXP readxl_xls_sheets(SEXP);
extern SEXP readxl_xlsx_date_formats(SEXP);
extern SEXP readxl_xlsx_sheets(SEXP);
extern SEXP readxl_xlsx_strings(SEXP);
extern SEXP readxl_zip_xml(SEXP, SEXP);

static const R_CallMethodDef CallEntries[] = {
  {"readxl_parse_ref",         (DL_FUNC) &readxl_parse_ref,         1},
  {"readxl_read_xls_",         (DL_FUNC) &readxl_read_xls_,         9},
  {"readxl_read_xlsx_",        (DL_FUNC) &readxl_read_xlsx_,        9},
  {"readxl_xls_date_formats",  (DL_FUNC) &readxl_xls_date_formats,  1},
  {"readxl_xls_sheets",        (DL_FUNC) &readxl_xls_sheets,        1},
  {"readxl_xlsx_date_formats", (DL_FUNC) &readxl_xlsx_date_formats, 1},
  {"readxl_xlsx_sheets",       (DL_FUNC) &readxl_xlsx_sheets,       1},
  {"readxl_xlsx_strings",      (DL_FUNC) &readxl_xlsx_strings,      1},
  {"readxl_zip_xml",           (DL_FUNC) &readxl_zip_xml,           2},
  {NULL, NULL, 0}
};

void R_init_readxl(DllInfo *dll)
{
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
