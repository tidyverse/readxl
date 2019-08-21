context("excel_sheets")

test_that("non-existent file throws error", {
  expect_error(excel_sheets("foo"), "`path` does not exist")
})

test_that("detected sheets are correct", {
  expect_identical(excel_sheets(test_sheet("blanks.xls")), c("different_rows", "same_row_first", "same_row_middle"))
})

test_that("invalid pattern throws error", {
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), glob = 123), "`glob` must be a string.")
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), glob = TRUE), "`glob` must be a string.")
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), glob = c("this", "that")), "`glob` must be a string.")
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), glob = "nothing"), "can not find sheets with the specified criteria.")
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), regexp = 123), "`regexp` must be a string.")
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), regexp = TRUE), "`regexp` must be a string.")
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), regexp = c("this", "that")), "`regexp` must be a string.")
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), regexp = "nothing"), "can not find sheets with the specified criteria.")
  expect_error(excel_sheets(test_sheet("geometry.xlsx"), glob = "this", regexp = "that"), "use either `glob` or `regexp`, not both.")
})

test_that("filtering is working", {
  expect_identical(excel_sheets(test_sheet("geometry.xlsx"), glob = "warn*"), c("warning_B6", "warning_AT6", "warning_AKE6"))
  expect_identical(excel_sheets(test_sheet("geometry.xlsx"), glob = "warning*"), c("warning_B6", "warning_AT6", "warning_AKE6"))
  expect_identical(excel_sheets(test_sheet("geometry.xlsx"), regexp =  "^warn"), c("warning_B6", "warning_AT6", "warning_AKE6"))
  expect_identical(excel_sheets(test_sheet("geometry.xlsx"), regexp =  "_"), c("warning_B6", "warning_AT6", "warning_AKE6"))
  expect_identical(excel_sheets(test_sheet("geometry.xlsx"), regexp =  "^[A-Z]"), "Sheet1")
  expect_identical(excel_sheets(test_sheet("blanks.xlsx"), regexp =  "_row_"), c("same_row_first", "same_row_middle"))
})
