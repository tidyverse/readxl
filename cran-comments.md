## Test environments

* local OS X install, R 3.3.2
* Ubuntu precise (12.04.5 LTS) via travis-ci
  - R-oldrel = R 3.2.5 
  - R-release = R 3.3.3
  - R-devel = R (unstable) (2017-04-11 r72505)
* Ubuntu 14.04.5 LTS + R 3.3.3 via travis-ci
* macOS Sierra 10.12.1 + R 3.3.3 via travis-ci
* OS X El Capitan 10.11.6 + R 3.3.3 via travis-ci
* Windows Server 2012 + R 3.3.3 via appveyor
* Windows + R 3.4.0 RC (2017-04-13 r72510) via win-builder

## R CMD check results

This is a resubmission to move the license information for the wrapped libxls library from LICENSE to LICENSE.note, as per Kurt Hornik's instructions. No other changes have been made.

There are no errors or warnings.

There is one NOTE, that makes two points:

  * Change of maintainer: from Hadley Wickham to Jennifer Bryan.
  * The LICENSE file specifies precisely the license of the included libxls code and is unchanged since the previous readxl release.

Two possibly invalid URLs are flagged, one in NEWS.md and another in a vignette. Both are valid URLs below https://support.microsoft.com that I have no trouble accessing. I'm not sure why they are flagged.

This package fails on big-endian platforms due to endian-ness bugs in the wrapped libxls library. This has always been the case and nothing has changed.

## Reverse dependencies

There are 39 reverse dependencies, 37 of which checked successfully (no errors or warnings; notes unrelated to readxl). Here is a report for the 35 checks that could be automated:
https://github.com/tidyverse/readxl/blob/master/revdep/README.md.

These two had error(s):

1. elementR. 1 error, in an example, which is also showing on this package's CRAN Package Check Results. No reason to believe this is connected to readxl.
  
2. heemod. 1 test failure, which is also showing on this package's CRAN Package Check Results. No reason to believe this is connected to readxl.
