context("Problems")

test_that("coercion warnings report correct address", {
  ## these sheets have a blank first column
  ## make sure warning indicates position is spreadsheet (column 2)
  ## vs position in the data frame we're building (column 1)

  ## xlsx
  expect_warning(
    read_excel(test_sheet("geometry.xlsx"), sheet = "warning_position",
               col_types = "numeric"),
    "Expecting numeric in [6, 2]",
    fixed = TRUE
  )
  ## xls
  expect_warning(
    read_excel(test_sheet("geometry.xls"), sheet = "warning_position",
               col_types = "numeric"),
    "Expecting numeric in [6, 2]",
    fixed = TRUE
  )
})
