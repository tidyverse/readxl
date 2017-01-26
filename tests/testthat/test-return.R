context("read_excel")

test_that("return type for xlsx files", {
  types <- read_excel(test_sheet("missing-values.xlsx"))
  expect_is(types, "tbl_df")
  expect_is(types, "tbl")
  expect_is(types, "data.frame")
  expect_false(tibble::has_rownames(types))
})

test_that("return type for xls files", {
  types <- read_excel(test_sheet("missing-values.xls"))
  expect_is(types, "tbl_df")
  expect_is(types, "tbl")
  expect_is(types, "data.frame")
  expect_false(tibble::has_rownames(types))
})
