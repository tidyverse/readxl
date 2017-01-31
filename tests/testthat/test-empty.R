context("Empty sheets")

test_that("completely empty sheets are handled", {
  expect_warning(
    out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty"),
    "No <row> in sheet xml. No data read."
  )
  expect_identical(out, tibble::tibble())
})

test_that("sheets with column names only are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "header_only")
  expect_identical(out, tibble::tibble(var1 = numeric(), var2 = numeric()))
})
