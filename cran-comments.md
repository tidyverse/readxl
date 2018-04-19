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

There are no errors, but there is one warning (see below). On some platforms, but not all, I see this NOTE:

  installed size is  5.2Mb
  sub-directories of 1Mb or more:
    libs   3.8Mb
    
This NOTE is not new and can currently be seen for the CRAN version on r-devel-linux-x86_64-fedora-gcc.

I see a warning originating from libxls/xslstruct.h about the use of a zero size and variable length arrays. We are actively working with the maintainer of libxls to eliminate this practice, so that libxls will compile with no warnings, with a C++ compiler and CRAN's flags. However, there is some urgency to get this version of libxls/readxl onto CRAN in the meantime:

The main reason for this release is to include an updated version of the embedded libxls library, which has been patched to address these CVEs:

https://www.talosintelligence.com/vulnerability_reports/TALOS-2017-0462
https://www.talosintelligence.com/vulnerability_reports/TALOS-2017-0403
https://www.talosintelligence.com/vulnerability_reports/TALOS-2017-0404

FYI the same firm also posted two other CVEs that mention readxl, but then go on clarify that the vulnerabilities do not actually affect readxl (they have also been fixed in libxls for years):

https://www.talosintelligence.com/vulnerability_reports/TALOS-2017-0463
https://www.talosintelligence.com/vulnerability_reports/TALOS-2017-0426

readxl fails on big-endian platforms due to endian-ness bugs in the wrapped libxls library. This has always been the case and nothing has changed.

## Reverse dependencies

I attempted to check all 67 reverse dependencies and succeeded with 65. I compared check results between the current CRAN version of readxl and this submission.

For these 65, there are no notes, warnings or errors that appear to be related to readxl, nor any that differ between checks run with readxl v1.0.0 and v1.1.0.

I was unable to check 2 packages (rattle, Ricetl) due to onerous installation requirements.
