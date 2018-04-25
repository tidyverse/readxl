#ifndef __READ_XLS_CRAN_H
#define __READ_XLS_CRAN_H

/* Mask printf for CMD check */
#include <Rinternals.h>
#define printf Rprintf

/* Rather than use variadic macros (which are a C99 feature) we are going to
 * hard code macros with different numbers of arguments
 */


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

#undef assert
#define assert(X) if (X) {}
#define exit assert

#endif
