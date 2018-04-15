## usage:
## test_sheet("blanks.xls")
test_sheet <- function(fname) rprojroot::find_testthat_root_file("sheets", fname)

## once https://github.com/hadley/testthat/commit/c83aba9 is on CRAN
## can use testthat::test_path() for this, i.e.,
# test_sheet <- function(fname) testthat::test_path("sheets", fname)
