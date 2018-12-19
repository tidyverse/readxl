## Test environments

* local macOS Mojave 10.14.1 + R 3.5.1
* macOS Sierra 10.12.6 + R 3.5.1 via travis-ci
* OS X El Capitan 10.11.6 + R 3.5.1 via travis-ci
* Ubuntu trusty (14.04.5 LTS) via travis-ci
  - R 3.1.3, 3.2.5, 3.3.3
  - R-oldrel = R 3.4.4
  - R-release = R 3.5.1
  - R-devel = (unstable) (2018-12-19 r75866)
* local Windows 10 VM, R 3.5.1
* Windows Server 2012 + 3.5.1 Patched (2018-12-08 r75805) via appveyor
* Windows + R 3.5.1 & Under development (unstable) (2018-12-17 r75857) via win-builder

## R CMD check results

I see no errors, warnings, or notes for readxl.

The current CRAN results for clang-UBSAN and gcc-UBSAN note a misaligned address for type 'DWORD', emanating from the embedded libxls library. I believe I have gotten this fixed upstream and, therefore, in the current submission.

The main reason for this release is to prepare for a coming release of the tibble package, which readxl depends on.

readxl fails on big-endian platforms due to endian-ness bugs in the wrapped libxls library. This has always been the case and nothing has changed.

## Reverse dependencies

I attempted to check all 88 reverse dependencies (CRAN and BioC) and succeeded with 83. I compared check results between the current CRAN version of readxl and this submission.

For these 83, there are no notes, warnings or errors that appear to be related to readxl, nor any that differ between checks run with readxl v1.1.0 and v1.2.0.

I was unable to check 5 packages (DAPAR, dynBiplotGUI, lpirfs, PCRedux, zooaRchGUI) for various combinations of these reasons: installation/compilation failure, R CMD check timed out, missing external dependencies.
