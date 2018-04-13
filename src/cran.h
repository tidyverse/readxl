#ifndef __READ_XLS_CRAN_H
#define __READ_XLS_CRAN_H

/* Mask printf for CMD check */
#include <Rinternals.h>
#define printf Rprintf

/* Rather than use variadic macros (which are a C99 feature) we are going to
 * hard code macros with different numbers of arguments
 */
#define fprintf2(err, msg) Rprintf(msg)
#define fprintf3(err, msg, a1) Rprintf(msg, a1)
#define fprintf4(err, msg, a1, a2) Rprintf(msg, a1, a2)
#define fprintf5(err, msg, a1, a2, a3) Rprintf(msg, a1, a2, a3)
#define fprintf10(err, msg, a1, a2, a3, a4, a5, a6, a7, a8) Rprintf(msg, a1, a2, a3, a4, a5, a6, a7, a8)

#undef assert
#define assert(X) if (X) {}
#define exit assert

#endif
