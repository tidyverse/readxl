context("Dates")

test_that("dates respect worksheet date setting", {
  nms <- paste0("X", 1:5)
  d1900 <- read_xls(test_sheet("dates-1900.xls"), col_names = nms)
  d1904 <- read_xls(test_sheet("dates-1904.xls"), col_names = nms)
  d1900loo <- read_xlsx(test_sheet("dates-1900-LibreOffice.xlsx"), col_names = nms)

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

## Lotus 1-2-3 leap year bug
## #264, #148
test_that("we get correct dates prior to March 1, 1900, in 1900 date system", {
  ## xlsx
  expect_warning(
    df <- read_excel(
      test_sheet("dates-leap-year-1900-xlsx.xlsx"),
      col_types = c("date", "text", "logical")
    ),
    "NA inserted for impossible 1900-02-29 datetime"
  )
  dttms <- as.POSIXct(df$dttm_string, format = "%Y-%m-%d %H:%M:%S", tz = "UTC")
  leap_day <- df$dttm_string == "1900-02-29 08:00:00"
  expect_identical(df$dttm[!leap_day], dttms[!leap_day])
  expect_true(is.na(df$dttm[leap_day]))

  ## xls
  expect_warning(
    df <- read_excel(
      test_sheet("dates-leap-year-1900-xls.xls"),
      col_types = c("date", "text", "logical")
    ),
    "NA inserted for impossible 1900-02-29 datetime"
  )
  dttms <- as.POSIXct(df$dttm_string, format = "%Y-%m-%d %H:%M:%S", tz = "UTC")
  leap_day <- df$dttm_string == "1900-02-29 08:00:00"
  expect_identical(df$dttm[!leap_day], dttms[!leap_day])
  expect_true(is.na(df$dttm[leap_day]))
})
