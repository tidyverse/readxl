context("Colnames")

test_that("auto-generated column names are the same for xls and xlsx files", {
  xls <- read_xls("missing-values.xls", col_names = FALSE)
  xlsx <- read_xlsx("missing-values.xlsx", col_names = FALSE)

  expect_equal(colnames(xls), colnames(xlsx))
})
