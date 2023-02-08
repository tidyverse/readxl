## usage:
## test_sheet("blanks.xls")
test_sheet <- function(fname) testthat::test_path("sheets", fname)
