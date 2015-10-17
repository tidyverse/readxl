context("read_excel")

test_that("types imputed & read correctly", {
  types <- read_excel(test_sheet("types.xlsx"), n = 120)
  expect_is(types$number, "numeric")
  expect_is(types$string, "character")
  expect_is(types$date, "POSIXct")
  expect_is(types$string_at_102, "character")
  expect_warning(
    read_excel(test_sheet("types.xlsx")),
    "expecting numeric")
  expect_silent(read_excel(test_sheet("types.xlsx"), n = 120))
})

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
