context("Dates")

test_that("dates respsect worksheet date setting", {
  d1900 <- read_xls("dates-1900.xls", col_names = FALSE)
  d1904 <- read_xls("dates-1904.xls", col_names = FALSE)

  expect_equal(d1900, d1904)
  expect_equal(d1900$X1, ISOdate(2000, 01, 01, 0, tz = "UTC"))
})
