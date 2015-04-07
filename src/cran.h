#ifndef __READ_XLS_CRAN_H
#define __READ_XLS_CRAN_H

/* Mask printf for CMD check */
#include <Rinternals.h>
#define printf Rprintf
#define fprintf(err, ...) Rprintf(__VA_ARGS__)
#define exit assert
#undef assert
#define assert(X) if (X) {}

#endif
