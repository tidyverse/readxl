test_that("can read sheets with inlineStr", {
  # Original source: http://our.componentone.com/wp-content/uploads/2011/12/TestExcel.xlsx
  # These appear to come from LibreOffice 4.2.7.2.
  x <- read_excel(test_sheet("inlineStr.xlsx"))
  expect_equal(x$ID, "RQ11610")
})

test_that("can read files with and without extension [xls]", {
  iris_xls_no_ending <- withr::local_tempfile()
  file.copy(test_sheet("iris-excel-xls.xls"), iris_xls_no_ending)
  iris_xls <- read_excel(test_sheet("iris-excel-xls.xls"))
  iris_guess <- read_excel(iris_xls_no_ending)
  iris_explicit <- read_xls(iris_xls_no_ending)
  expect_equal(iris_xls, iris_guess)
  expect_equal(iris_xls, iris_explicit)
})

test_that("xlsx is not read as xls and vice versa", {
  expect_snapshot(
    error = TRUE,
    read_xls(test_sheet("iris-excel-xlsx.xlsx"))
  )
  expect_snapshot(
    error = TRUE,
    read_xlsx(test_sheet("iris-excel-xls.xls")),
  )
})

test_that("non-existent file throws error", {
  expect_snapshot(
    error = TRUE,
    read_excel("foo")
  )
})

test_that("read_excel catches invalid guess_max", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = NA)
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = -1)
  )
  expect_snapshot(
    out <- read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = Inf)
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = NULL)
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = 1:2)
  )
})

test_that("read_excel catches invalid n_max", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = NA)
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = -1)
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = NULL)
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = 1:2)
  )
})

## https://github.com/tidyverse/readxl/issues/373
test_that("xls with exactly 65536 rows does not enter infinite loop", {
  out <- read_excel(test_sheet("65536-rows-xls.xls"))
  expect_identical(out, tibble::tibble(HELLO = "WORLD"))
})

test_that("sheet must be integer or string", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("mtcars.xls"), sheet = TRUE)
  )
})

test_that("trim_ws must be a logical", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("mtcars.xls"), trim_ws = "yes")
  )
})

## xlsx: https://github.com/tidyverse/readxl/issues/370
## xls:  https://github.com/tidyverse/readxl/issues/476
test_that("non-ASCII filenames can be read", {
  skip_on_cran()
  ## chosen to be non-ASCII but
  ## [1] representable in Windows-1252 and
  ## [2] not any of the few differences between Windows-1252 and ISO-8859-1
  ## a-grave + e-diaeresis  + Eth + '.xls[x]'
  xls_filename <- "\u00C0\u00CB\u00D0\u002E\u0078\u006C\u0073"
  xlsx_filename <- "\u00C0\u00CB\u00D0\u002E\u0078\u006C\u0073\u0078"
  tricky_xls_file <- file.path(tempdir(), xls_filename)
  tricky_xlsx_file <- file.path(tempdir(), xlsx_filename)
  file.copy(test_sheet("list_type.xls"), tricky_xls_file)
  file.copy(test_sheet("list_type.xlsx"), tricky_xlsx_file)
  expect_true(file.exists(tricky_xls_file))
  expect_true(file.exists(tricky_xlsx_file))
  on.exit(file.remove(tricky_xls_file))
  on.exit(file.remove(tricky_xlsx_file))
  expect_no_error(read_xls(tricky_xls_file))
  expect_no_error(read_xlsx(tricky_xlsx_file))
})

test_that("styles and sharedStrings parts can be absent", {
  expect_no_error(
    df <- read_xlsx(test_sheet("no-styles-or-sharedStrings-parts.xlsx"))
  )
  expect_identical(df$Language[1], "german")
  expect_true(all(df$Age > 0))
})
