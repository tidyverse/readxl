## Test environments

* local macOS Mojave 10.14.2 + R 3.5.2
* macOS Sierra 10.12.6 + R 3.5.2 via travis-ci
* OS X El Capitan 10.11.6 + R 3.5.2 via travis-ci
* Ubuntu trusty (14.04.5 LTS) via travis-ci
  - R 3.1.3, 3.2.5, 3.3.3
  - R-oldrel = R 3.4.4
  - R-release = R 3.5.2
  - R-devel = (unstable) (2019-03-09 r76216)
* local Windows 10 VM, R 3.5.2
* Windows Server 2012 + R 3.5.3 Patched (2019-03-11 r76221) via appveyor
* Windows + R Under development (unstable) (2019-03-12 r76226) via win-builder

## R CMD check results

The only reason for this release is adapt a few tests for a patch release of tibble that is imminent. Otherwise the only change since readxl v1.3.0 is that I updated a few URLs.

Repeating an explanation from my v1.3.0 submission:

There is a memo from clang-UBSAN memtests from the UndefinedBehaviorSanitizer. There's an instance of "outside the range of representable values of type 'int'", emanating from embedded libxls code. I've reported this upstream and we've all taken a look at it. This code has been present in this exact form for at least one readxl release and in equivalent form since readxl first appeared on CRAN. This bit of libxls code will likely be reworked in the next libxls release. I believe there is nothing urgent about it. readxl does not access the data affected by this.

## Reverse dependencies

I did not rerun revdeps, since I did it so recently and no readxl code has changed. I repeat results from the recent v1.3.0 release of readxl.

## revdepcheck results

We checked 91 reverse dependencies (83 from CRAN + 8 from BioConductor), comparing R CMD check results across CRAN and dev versions of this package.

 * We saw 0 new problems
 * We failed to check 3 packages

Issues with CRAN packages are summarised below.

### Failed to check

* dynBiplotGUI (check timed out)
* lpirfs       (failed to install)
* zooaRchGUI   (failed to install)
