## Test environments

* local macOS Mojave 10.14.2 + R 3.5.2
* macOS Sierra 10.12.6 + R 3.5.2 via travis-ci
* OS X El Capitan 10.11.6 + R 3.5.2 via travis-ci
* Ubuntu trusty (14.04.5 LTS) via travis-ci
  - R 3.1.3, 3.2.5, 3.3.3
  - R-oldrel = R 3.4.4
  - R-release = R 3.5.2
  - R-devel = (unstable) (2019-02-12 r76095)
* local Windows 10 VM, R 3.5.2
* Windows Server 2012 + 3.5.2 Patched (2019-02-05 r76086) via appveyor
* Windows + R 3.5.2 & Under development (unstable) (2019-02-11 r76086) via win-builder

## R CMD check results

The main reason for this release is to embed an updated version of libxls. It has recently had its first official release in years (v1.5.0) and this release addresses two recently reported CVEs:

  * CVE-2018-20452 http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2018-20452
  * CVE-2018-20450 http://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2018-20450

I see no errors, warnings, or notes for readxl. On some platforms, some of the time, we get this NOTE. This has always been the case; it's just a fact that we embed some fairly large libraries to parse both xls and xlsx.

N  checking installed package size
     installed size is XMb
     sub-directories of 1Mb or more:
       libs   YMb

The current CRAN results show a NOTE on some platforms about "Namespace in Imports field not imported from: 'progress'". I now list 'progress' only in LinkingTo (no longer in Imports), which reflects how it's actually used.

There is a memo from clang-UBSAN memtests from the UndefinedBehaviorSanitizer. There's an instance of "outside the range of representable values of type 'int'", emanating from embedded libxls code. I've reported this upstream and we've all taken a look at it. This code has been present in this exact form for at least one readxl release and in equivalent form since readxl first appeared on CRAN. This bit of libxls code may eventually be reworked but I believe there is nothing urgent about it.

## Reverse dependencies

## revdepcheck results

We checked 91 reverse dependencies (83 from CRAN + 8 from BioConductor), comparing R CMD check results across CRAN and dev versions of this package.

 * We saw 0 new problems
 * We failed to check 3 packages

Issues with CRAN packages are summarised below.

### Failed to check

* dynBiplotGUI (check timed out)
* lpirfs       (failed to install)
* zooaRchGUI   (failed to install)
