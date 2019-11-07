## usage:
## test_sheet("blanks.xls")
test_sheet <- function(fname) testthat::test_path("sheets", fname)

expect_error_free <- function(...) {
  expect_error(..., regexp = NA)
}
