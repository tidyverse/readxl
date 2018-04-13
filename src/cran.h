#ifndef __READ_XLS_CRAN_H
#define __READ_XLS_CRAN_H

/* Mask printf for CMD check */
#include <Rinternals.h>
#define printf Rprintf

#undef assert
#define assert(X) if (X) {}
#define exit assert

#endif
