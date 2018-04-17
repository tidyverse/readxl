## Test environments

* local macOS Sierra 10.12.6 + R 3.4.3
* macOS Sierra 10.12.6 + R 3.4.4 via travis-ci
* OS X El Capitan 10.11.6 + R 3.4.4 via travis-ci
* Ubuntu trusty (14.04.5 LTS) via travis-ci
  - R-oldrel = R 3.3.3
  - R-release = R 3.4.4
  - R-devel = R (unstable) (2018-04-16 r74611)
* local Windows 10 VM, R 3.4.3
* Windows Server 2012 + R 3.5.0 RC (2018-04-15 r74605) via appveyor
* Windows + R 3.4.4 & 3.5.0 beta (2018-04-13 r74592) via win-builder

## R CMD check results

There are no errors, warnings, or notes.

This package fails on big-endian platforms due to endian-ness bugs in the wrapped libxls library. This has always been the case and nothing has changed.

## Reverse dependencies

There are x reverse dependencies, y of which checked successfully (no errors or warnings; notes unrelated to readxl). Here is a report for the z checks that could be automated:
https://github.com/tidyverse/readxl/blob/master/revdep/README.md.

These a had error(s):

