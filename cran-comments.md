## Test environments

* local OS X install, R 3.3.2
* on travis-ci
  - Ubuntu precise (12.04.5 LTS) + R 3.2.5
  - Ubuntu precise (12.04.5 LTS) + R 3.3.3
  - Ubuntu precise (12.04.5 LTS) + R (unstable) (2017-04-11 r72505)
  - Ubuntu 14.04.5 LTS + R 3.3.3
  - macOS Sierra 10.12.1 + R 3.3.3
  - OS X El Capitan 10.11.6 + R 3.3.3
* on appveyor
  - Windows Server 2012 + R 3.3.3
* r-hub? do we get into that here?

## R CMD check results

0 errors | 0 warnings | 1 note

There will be a note about change of maintainer, at the very least. The rest of it is still a moving target.

## Reverse dependencies

There are 39 reverse dependencies, 37 of which checked successfully (no errors or warnings; notes unrelated to readxl). Here is a report for the 35 checks that could be automated:
https://github.com/tidyverse/readxl/blob/master/revdep/README.md.

These two had error(s):

1. elementR. 1 error, in an example, which is also showing on this package's CRAN Package Check Results. No reason to believe this is connected to readxl.
  
2. heemod. 1 test failure, which is also showing on this package's CRAN Package Check Results. No reason to believe this is connected to readxl.
