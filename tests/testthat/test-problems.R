context("Problems")

test_that("coercion warnings report correct address", {
  ## xlsx
  expect_warning(
    read_excel(
      test_sheet("geometry.xlsx"), sheet = "warning_B6",
      col_types = "numeric"
    ),
    "Expecting numeric in B6 / R6C2",
    fixed = TRUE
  )
  expect_warning(
    read_excel(
      test_sheet("geometry.xlsx"), sheet = "warning_AT6",
      col_types = "numeric"
    ),
    "Expecting numeric in AT6 / R6C46",
    fixed = TRUE
  )
  expect_warning(
    read_excel(
      test_sheet("geometry.xlsx"), sheet = "warning_AKE6",
      col_types = "numeric"
    ),
    "Expecting numeric in AKE6 / R6C967",
    fixed = TRUE
  )
  ## xls
  expect_warning(
    read_excel(
      test_sheet("geometry.xls"), sheet = "warning_B6",
      col_types = "numeric"
    ),
    "Expecting numeric in B6 / R6C2",
    fixed = TRUE
  )
  expect_warning(
    read_excel(
      test_sheet("geometry.xls"), sheet = "warning_AT6",
      col_types = "numeric"
    ),
    "Expecting numeric in AT6 / R6C46",
    fixed = TRUE
  )
  ## xls column max is 256, so we're done
})
