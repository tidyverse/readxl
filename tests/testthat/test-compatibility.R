context("Compatibility")

test_that("can read document from google doc", {
  iris_1 <- read_excel(test_sheet("iris-excel.xlsx"))
  iris_2 <- read_excel(test_sheet("iris-google-doc.xlsx"))

  expect_equal(iris_1, iris_2)
})

## #180, #152, #99
## Some 3rd-party s/w writes xls where lastcol is 0-indexed, like lastrow
## Changed an inequality in xls_cell() in  xls.c to accomodate this
## WriteXLS is (or, rather, wraps) such s/w, so it's good source of such xls.
## WriteXLS::WriteXLS(head(mtcars), file.path("tests", "testthat", "sheets", "mtcars.xls"))
test_that("can tolerate xls that underreports number of columns", {
  df <- read_excel(test_sheet("mtcars.xls"))
  expect_identical(ncol(df), ncol(mtcars))
})
