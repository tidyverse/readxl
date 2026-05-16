This release is motivated by the need to require cpp11 > 0.5.5 in order to skip over version 0.5.4 which causes readxl to segfault when warning about coercion issues (#784).

## revdepcheck results

We checked 521 reverse dependencies (520 from CRAN + 1 from Bioconductor), comparing R CMD check results across CRAN and dev versions of this package.

 * We saw 0 new problems
 * We failed to check 0 packages
