context("Empty sheets")

test_that("completely empty sheets are handled [xlsx]", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", skip = 3)
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", col_names = "a")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", col_names = FALSE)
  expect_identical(out, tibble::tibble())
})

test_that("completely empty sheets are handled [xls]", {
  out <- read_excel(test_sheet("empty-sheets.xls"), "empty")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xls"), "empty", skip = 3)
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xls"), "empty", col_names = "a")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xls"), "empty", col_names = FALSE)
  expect_identical(out, tibble::tibble())
})

test_that("sheets with column names only are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "header_only")
  expect_identical(out, tibble::tibble(var1 = logical(), var2 = logical()))
  out <- read_excel(test_sheet("empty-sheets.xls"), "header_only")
  expect_identical(out, tibble::tibble(var1 = logical(), var2 = logical()))
})

test_that("non-empty sheets act that way if we skip past everything", {
  out <- read_excel(test_sheet("skipping.xlsx"), skip = 10)
  expect_identical(out, tibble::tibble())
  out <- read_excel(test_sheet("skipping.xls"), skip = 10)
  expect_identical(out, tibble::tibble())
})
