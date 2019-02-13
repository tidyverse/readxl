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

There is a memo from clang-UBSAN memtests from the UndefinedBehaviorSanitizer. There's an instance of "outside the range of representable values of type 'int'", emanating from embedded libxls code. FILL THIS IN WHEN STORY CONCLUDES.

## Reverse dependencies

I attempted to check all 88 reverse dependencies (CRAN and BioC) and succeeded with 83. I compared check results between the current CRAN version of readxl and this submission.

For these 83, there are no notes, warnings or errors that appear to be related to readxl, nor any that differ between checks run with readxl v1.1.0 and v1.2.0.

I was unable to check 5 packages (DAPAR, dynBiplotGUI, lpirfs, PCRedux, zooaRchGUI) for various combinations of these reasons: installation/compilation failure, R CMD check timed out, missing external dependencies.
