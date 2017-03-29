context("read_excel")

test_that("can read sheets with inlineStr", {
  # Original source: http://our.componentone.com/wp-content/uploads/2011/12/TestExcel.xlsx
  # These appear to come from LibreOffice 4.2.7.2.
  x <- read_excel(test_sheet("inlineStr.xlsx"))
  expect_equal(x$ID, "RQ11610")
})

test_that("can read file without extension", {

  ## xlsx
  file.copy(test_sheet("iris-excel.xlsx"), "iris-xlsx-no-ending")
  on.exit(file.remove("iris-xlsx-no-ending"))
  iris_xlsx <- read_xlsx("iris-xlsx-no-ending")
  expect_equal(iris_xlsx, read_excel(test_sheet("iris-excel.xlsx")))
  expect_error(read_excel("iris-xlsx-no-ending"), "Missing file extension.")
  expect_error(
    expect_output(
      read_xls("iris-xlsx-no-ending"),
      "Not an excel file"
    ),
    "Failed to open iris-xlsx-no-ending"
  )

  ## xls
  file.copy(test_sheet("iris-excel.xls"), "iris-xls-no-ending")
  on.exit(file.remove("iris-xls-no-ending"), add = TRUE)
  iris_xls <- read_xls("iris-xls-no-ending")
  expect_equal(iris_xls, read_excel(test_sheet("iris-excel.xls")))
  expect_error(read_excel("iris-xls-no-ending"), "Missing file extension.")
  expect_error(read_xlsx("iris-xls-no-ending"), "cannot be opened")

})

test_that("read_excel catches invalid guess_max", {

  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), guess_max = NA),
    "`guess_max` must be a positive integer"
  )
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), guess_max = -1),
    "`guess_max` must be a positive integer"
  )
  expect_warning(
    read_excel(test_sheet("iris-excel.xlsx"), guess_max = Inf),
    "`guess_max` is a very large value"
  )
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), guess_max = NULL),
    "`guess_max` must be a positive integer"
  )
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), guess_max = 1:2),
    "`guess_max` must be a positive integer"
  )
})

test_that("read_excel only accepts file exts xlsx and xls", {
  expect_error(read_excel("foo.txt"), "Unknown file extension")
})

test_that("read_excel catches invalid n_max", {

  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), n_max = NA),
    "`n_max` must be a positive integer"
  )
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), n_max = -1),
    "`n_max` must be a positive integer"
  )
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), n_max = NULL),
    "`n_max` must be a positive integer"
  )
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), n_max = 1:2),
    "`n_max` must be a positive integer"
  )
})
