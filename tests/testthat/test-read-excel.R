context("read_excel")

test_that("types imputed & read correctly", {
  types <- read_excel("types.xlsx", n = 120)
  expect_is(types$number, "numeric")
  expect_is(types$string, "character")
  expect_is(types$date, "POSIXct")
  expect_is(types$string_at_102, "character")
  expect_warning(
    df <- read_excel("types.xlsx"),
    "expecting numeric")
  expect_silent(read_excel("types.xlsx", n = 120))
})

test_that("can read sheets with inlineStr", {
  # Original source: http://our.componentone.com/wp-content/uploads/2011/12/TestExcel.xlsx
  # These appear to come from LibreOffice 4.2.7.2.
  x <- read_excel("inlineStr.xlsx")
  expect_equal(x$ID, "RQ11610")
})
