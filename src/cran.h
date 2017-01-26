#ifndef __READ_XLS_CRAN_H
#define __READ_XLS_CRAN_H

/* Mask printf for CMD check */
#include <Rinternals.h>
#define printf Rprintf

/* Rather than use variadic macros (which are a C99 feature) we are going to
 * hard code a macro with only two additional arguments, as the only usage of
 * fprintf uses two additional arguments.
 */
#define fprintf(err, msg, a1, a2) Rprintf(msg, a1, a2)
#define exit assert
#undef assert
#define assert(X) if (X) {}

#endif
