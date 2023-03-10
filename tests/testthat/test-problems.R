test_that("coercion warnings report correct address", {
  ## xlsx
  expect_snapshot(
    read_excel(
      test_sheet("geometry.xlsx"),
      sheet = "warning_B6",
      col_types = "numeric"
    )
  )
  expect_snapshot(
    read_excel(
      test_sheet("geometry.xlsx"),
      sheet = "warning_AT6",
      col_types = "numeric"
    )
  )
  expect_snapshot(
    read_excel(
      test_sheet("geometry.xlsx"),
      sheet = "warning_AKE6",
      col_types = "numeric"
    )
  )
  ## xls
  expect_snapshot(
    read_excel(
      test_sheet("geometry.xls"),
      sheet = "warning_B6",
      col_types = "numeric"
    )
  )
  expect_snapshot(
    read_excel(
      test_sheet("geometry.xls"),
      sheet = "warning_AT6",
      col_types = "numeric"
    )
  )
  ## xls column max is 256, so we're done
})
