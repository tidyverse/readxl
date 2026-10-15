#include "cran.h"

void Rprintf2(FILE * stream, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  REvprintf(format, ap);
  va_end(ap);
}
