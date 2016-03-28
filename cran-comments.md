## Test environments
* local OS X install, R 3.2.4
* ubuntu 12.04 (on travis-ci), R 3.2.4
* win-builder (devel and release)

## R CMD check results

0 errors | 0 warnings | 1 note

* The LICENSE file specifies precisely the license of the included libxls
  code.

The package still fails on solaris, due to endian-ness bugs in the wrapped libxls library.

## Reverse dependencies

I did not run R CMD check on the reverse dependencies because this release only fixes minor R CMD check notes, and a test failure illuminated by the dev version of testthat.
