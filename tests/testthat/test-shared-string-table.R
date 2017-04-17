context("Shared string table")

## #293, #333
## affects xls only
test_that("shared strings are retrieved when there's >256 unique strings", {
  df <- read_excel(test_sheet("more-than-256-unique-strings-xls.xls"))
  expect_identical(df$var[257], "x257")
})
