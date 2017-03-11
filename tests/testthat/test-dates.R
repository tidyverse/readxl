context("Dates")

test_that("dates respect worksheet date setting", {
  nms <- paste0("X", 1:5)
  d1900 <- read_xls(test_sheet("dates-1900.xls"), col_names = nms)
  d1904 <- read_xls(test_sheet("dates-1904.xls"), col_names = nms)
  d1900loo <- read_xlsx(test_sheet("dates-1900-loo.xlsx"), col_names = nms)

  expect_equal(d1900, d1904)
  expect_equal(d1900, d1900loo)
  expect_equal(d1900$X1, ISOdate(2000, 01, 01, 0, tz = "UTC"))
})

## #187 Date/times missing a second
test_that("date subsecond rounding works", {
  ## xlsx
  df <- read_excel(test_sheet("datetime-rounding.xlsx"))
  expect_identical(as.character(df$dttm), df$dttm_string)

  ## xls
  df <- read_excel(test_sheet("datetime-rounding.xls"))
  expect_identical(as.character(df$dttm), df$dttm_string)
})
