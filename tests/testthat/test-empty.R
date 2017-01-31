context("Empty sheets")

test_that("completely empty sheets are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", skip = 3)
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", col_names = "a")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", col_names = FALSE)
  expect_identical(out, tibble::tibble())
})

test_that("sheets with column names only are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "header_only")
  expect_identical(out, tibble::tibble(var1 = numeric(), var2 = numeric()))
})

test_that("non-empty sheets act that way if we skip past everything", {
  ## currently we get a 0-row tibble with 2 unnamed columns
  ## a big improvement but when I rationlize column names, see why these
  ## empty column names aren't causing the columns to be dropped and fix it
  out <- read_excel(test_sheet("skipping.xlsx"), skip = 10)
  expect_is(out, "tbl_df")
  expect_identical(nrow(out), 0L)
  skip("figure out why empty unnamed columns aren't dropped and update test")
})
