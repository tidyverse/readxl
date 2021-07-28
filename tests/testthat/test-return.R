test_that("return type for xlsx files", {
  types <- read_excel(test_sheet("missing-values-xlsx.xlsx"))
  expect_s3_class(types, "tbl_df")
  expect_s3_class(types, "tbl")
  expect_s3_class(types, "data.frame")
  expect_false(tibble::has_rownames(types))
})

test_that("return type for xls files", {
  types <- read_excel(test_sheet("missing-values-xls.xls"))
  expect_s3_class(types, "tbl_df")
  expect_s3_class(types, "tbl")
  expect_s3_class(types, "data.frame")
  expect_false(tibble::has_rownames(types))
})
