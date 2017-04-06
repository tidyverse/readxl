context("Skipping")

skipping_xlsx <- test_sheet("skipping.xlsx")
skipping_xls <- test_sheet("skipping.xls")
ref <- tibble::tribble(
  ~ var1, ~ var2,
      NA,     NA,
  "v2,1", "v2,2",
      NA,     NA,
  "v4,1", "v4,2"
)

test_that("leading blank rows are implicitly skipped", {
  xlsx <- read_excel(skipping_xlsx, sheet = "two_blank_rows")
  xls <- read_excel(skipping_xls, sheet = "two_blank_rows")
  expect_identical(xlsx, ref)
  expect_identical(xls, ref)
})

test_that("leading blank rows can be explicitly skipped", {
  xlsx <- read_excel(skipping_xlsx, sheet = "two_blank_rows", skip = 2)
  xls <- read_excel(skipping_xls, sheet = "two_blank_rows", skip = 2)
  expect_identical(xlsx, ref)
  expect_identical(xls, ref)
})

test_that("leading blank rows can be implicitly AND explicitly skipped", {
  xlsx <- read_excel(skipping_xlsx, sheet = "two_blank_rows", skip = 1)
  xls <- read_excel(skipping_xls, sheet = "two_blank_rows", skip = 1)
  expect_identical(xlsx, ref)
  expect_identical(xls, ref)
})

test_that("failure to skip junk leads to garbage df but no error", {
  out <- read_excel(skipping_xlsx, sheet = "occupied_row_and_blank_row")
  expect_identical(out$`blah blah`, c(NA, "var1", NA, "v2,1", NA, "v4,1"))
  expect_identical(out[[2]], c(NA, "var2", NA, "v2,2", NA, "v4,2"))

  out <- read_excel(skipping_xls, sheet = "occupied_row_and_blank_row")
  expect_identical(out$`blah blah`, c(NA, "var1", NA, "v2,1", NA, "v4,1"))
  expect_identical(out[[2]], c(NA, "var2", NA, "v2,2", NA, "v4,2"))

  out <- read_excel(skipping_xlsx, sheet = "two_occupied_rows")
  expect_identical(out$`blah blah`, c(NA, "var1", NA, "v2,1", NA, "v4,1"))
  expect_identical(out[[2]], c("yada yada", "var2", NA, "v2,2", NA, "v4,2"))

  out <- read_excel(skipping_xls, sheet = "two_occupied_rows")
  expect_identical(out$`blah blah`, c(NA, "var1", NA, "v2,1", NA, "v4,1"))
  expect_identical(out[[2]], c("yada yada", "var2", NA, "v2,2", NA, "v4,2"))

  out <- read_excel(skipping_xlsx, sheet = "two_occupied_rows", skip = 1)
  expect_identical(out[[1]], c("var1", NA, "v2,1", NA, "v4,1"))
  expect_identical(out$`yada yada`, c("var2", NA, "v2,2", NA, "v4,2"))

  out <- read_excel(skipping_xls, sheet = "two_occupied_rows", skip = 1)
  expect_identical(out[[1]], c("var1", NA, "v2,1", NA, "v4,1"))
  expect_identical(out$`yada yada`, c("var2", NA, "v2,2", NA, "v4,2"))
})

test_that("explicit skip of leading junk, implicit skip of blank rows", {
  xlsx <- read_excel(skipping_xlsx, sheet = "occupied_row_and_blank_row", skip = 1)
  xls <- read_excel(skipping_xls, sheet = "occupied_row_and_blank_row", skip = 1)
  expect_identical(xlsx, ref)
  expect_identical(xls, ref)
})

test_that("explicit skip of leading junk and blank rows", {
  xlsx <- read_excel(skipping_xlsx, sheet = "occupied_row_and_blank_row", skip = 2)
  xls <- read_excel(skipping_xls, sheet = "occupied_row_and_blank_row", skip = 2)
  expect_identical(xlsx, ref)
  expect_identical(xls, ref)
})

test_that("explicit skip of leading junk", {
  xlsx <- read_excel(skipping_xlsx, sheet = "two_occupied_rows", skip = 2)
  xls <- read_excel(skipping_xls, sheet = "two_occupied_rows", skip = 2)
  expect_identical(ref, xlsx)
  expect_identical(ref, xls)
})
