context("Empty sheets")

test_that("completely empty sheets are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty")
  expect_identical(out, tibble::tibble())
})

test_that("sheets with column names only are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "header_only")
  expect_identical(out, tibble::tibble(var1 = numeric(), var2 = numeric()))
})
