context("read_excel")

test_that("types imputed & read correctly", {
  types <- read_excel("types.xlsx")
  expect_is(types$number, "numeric")
  expect_is(types$string, "character")
  expect_is(types$date, "POSIXct")
})
