context("65536 rows")

test_that("sheets with 65536 rows are handled [xls]", {
  out <- read_excel(test_sheet("65536-rows.xls"))
  expect_identical(out, tibble::tibble(X=0))
})
