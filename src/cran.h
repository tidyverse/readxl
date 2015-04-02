/* Mask printf for CMD check */
#include <Rinternals.h>
#define printf Rprintf
#define fprintf(err, ...) Rprintf(__VA_ARGS__)
#define exit assert
