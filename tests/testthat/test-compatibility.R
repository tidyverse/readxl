context("Compatability")

test_that("can read document from google doc", {
  iris_1 <- read_excel(test_sheet("iris-excel.xlsx"))
  iris_2 <- read_excel(test_sheet("iris-google-doc.xlsx"))

  expect_equal(iris_1, iris_2)
})
