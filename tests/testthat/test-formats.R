context("test-formats.R")

test_that("excel_format() recognizes extensions 'xls' and 'xlsx'", {
  files <- c("a.xlsx", "b.xls", "c.png", "d")
  expect_identical(
    excel_format(files, guess = FALSE),
    c("xlsx", "xls", NA, NA)
  )
})

test_that("excel_format() detect file signature", {
  orig_files <- c(
    test_sheet("iris-excel-xlsx.xlsx"),
    test_sheet("iris-excel-xls.xls"),
    file.path(R.home("doc"), "html", "logo.jpg")
  )
  no_ext_copies <- paste0(
    tools::file_path_sans_ext(orig_files),
    "-no-ending"
  )
  file.copy(orig_files, no_ext_copies)
  on.exit(file.remove(no_ext_copies))
  expect_identical(excel_format(no_ext_copies), c("xlsx", "xls", NA))
})

test_that("can read files with and without extension [xlsx]", {
  file.copy(test_sheet("iris-excel-xlsx.xlsx"), "iris-xlsx-no-ending")
  on.exit(file.remove("iris-xlsx-no-ending"))
  iris_xlsx <- read_excel(test_sheet("iris-excel-xlsx.xlsx"))
  iris_guess <- read_excel("iris-xlsx-no-ending")
  iris_explicit <- read_xlsx("iris-xlsx-no-ending")
  expect_equal(iris_xlsx, iris_guess)
  expect_equal(iris_xlsx, iris_explicit)
})
