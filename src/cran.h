#ifndef __READ_XLS_CRAN_H
#define __READ_XLS_CRAN_H

/* Mask printf and fprintf for R CMD check */
#include <Rinternals.h>
#include <R_ext/Print.h> /* Rprintf, Rvprintf */
#define printf Rprintf

/* Also mask putchar/puts/vprintf so vendored msoffice debug code stays verbatim */
#undef putchar
#define putchar(c) Rprintf("%c", (c))
#undef puts
#define puts(s) Rprintf("%s\n", (s))
#define vprintf(fmt, ap) Rvprintf((fmt), (ap))

#ifdef __cplusplus
extern "C" {
#endif

void Rprintf2(FILE * stream, const char *format, ...);

#ifdef __cplusplus
}
#endif
#define fprintf Rprintf2
#undef stderr
#define stderr NULL

#endif
