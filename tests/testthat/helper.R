## usage:
## test_sheet("blanks.xls")
test_sheet <- function(fname) testthat::test_path("sheets", fname)

skip_if_no_decryption <- function() {
  skip_if_not(
    decryption_available(),
    "decryption is not available on this platform"
  )
}
