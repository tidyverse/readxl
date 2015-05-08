context("Colnames")

test_that("dates respsect worksheet date sheeting", {
  xls <- read_xls("missing-values.xls", col_names = FALSE)
  xlsx <- read_xlsx("missing-values.xlsx", col_names = FALSE)

  expect_equal(colnames(xls), colnames(xlsx))
})
