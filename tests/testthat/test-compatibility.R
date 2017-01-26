context("Compatability")

expect_that("can read document from google doc", {
  iris_1 <- read_excel("iris-excel.xlsx")
  iris_2 <- read_excel("iris-google-doc.xlsx")

  expect_equal(iris_1, iris_2)
})
