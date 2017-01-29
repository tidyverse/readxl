context("Empty sheets")

test_that("completely empty sheets are handled", {
  expect_warning(
    out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty"),
    "No <row> in sheet xml. No data read."
  )
  expect_identical(out, tibble::tibble())
})

test_that("sheets with column names only are handled", {
  ## eventually these won't error, but I haven't gotten to a point where
  ## I can fix that yet
  expect_error(read_excel(test_sheet("empty-sheets.xlsx"), "header_only"),
               "No data for row 2 or higher")
})
