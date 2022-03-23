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

## Lotus 1-2-3 leap year bug ----
## #264, #148
test_that("we get correct dates prior to March 1, 1900, in 1900 date system [xlsx]", {
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
})

## Lotus 1-2-3 leap year bug
## #264, #148
test_that("we get correct dates prior to March 1, 1900, in 1900 date system [xls]", {
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

# https://github.com/tidyverse/readxl/issues/388
test_that("colors in number formats aren't misinterpreted as dates", {
  # <numFmts count="8">
  #   <numFmt numFmtId="165" formatCode="[Red]0"/>
  #   <numFmt numFmtId="166" formatCode="[Black]0"/>
  #   <numFmt numFmtId="167" formatCode="[Green]0"/>
  #   <numFmt numFmtId="168" formatCode="[White]0"/>
  #   <numFmt numFmtId="169" formatCode="[Blue]0"/>
  #   <numFmt numFmtId="170" formatCode="[Magenta]0"/>
  #   <numFmt numFmtId="171" formatCode="[Yellow]0"/>
  #   <numFmt numFmtId="172" formatCode="[Cyan]0"/>
  # </numFmts>
  expect_warning(
    read_xlsx(
      test_sheet("color-date-xlsx.xlsx"),
      col_names = "X1",
      col_types = "numeric"
    ),
    NA # rather than "Expecting numeric in A1 / R1C1: got a date" etc.
  )
  expect_warning(
    read_xls(
      test_sheet("color-date-xls.xls"),
      col_names = "X1",
      col_types = "numeric"
    ),
    NA # rather than "Expecting numeric in A1 / R1C1: got a date"
  )

  # <numFmts count="8">
  #   <numFmt numFmtId="165" formatCode="[red]0"/>
  #   <numFmt numFmtId="166" formatCode="[black]0"/>
  #   <numFmt numFmtId="167" formatCode="[green]0"/>
  #   <numFmt numFmtId="168" formatCode="[white]0"/>
  #   <numFmt numFmtId="169" formatCode="[blue]0"/>
  #   <numFmt numFmtId="170" formatCode="[magenta]0"/>
  #   <numFmt numFmtId="171" formatCode="[yellow]0"/>
  #   <numFmt numFmtId="172" formatCode="[cyan]0"/>
  # </numFmts>
  expect_warning(
    read_xlsx(
      test_sheet("color-date-lowercase-xlsx.xlsx"),
      col_names = "X1",
      col_types = "numeric"
    ),
    NA # rather than "Expecting numeric in A1 / R1C1: got a date"
  )
})

# https://github.com/tidyverse/readxl/issues/633
test_that("Swiss Francs (CHF) aren't misinterpreted as dates", {
  # <numFmts count="2">
  #   <numFmt numFmtId="164" formatCode="#,##0.00\ [$CHF]"/>
  #   <numFmt numFmtId="165" formatCode="#,##0.00\ [$EUR]"/>
  # </numFmts>
  dat <- read_xlsx(test_sheet("currency-formats-xlsx.xlsx"))
  expect_true(is.numeric(dat$cost_EUR))
  expect_true(is.numeric(dat$cost_CHF))
  expect_true(is.numeric(dat$cost_mixed))
})
