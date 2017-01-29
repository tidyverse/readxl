context("Skipping")

skipping <- test_sheet("skipping.xlsx")
df <- tibble::tribble(~ var1, ~ var2,
                          11,     12,
                          21,     22)

test_that("leading blank rows are implicitly skipped", {
  out <- read_excel(skipping, sheet = "two_blank_rows")
  expect_identical(df, out)
})

test_that("leading blank rows can be explicitly skipped", {
  out <- read_excel(skipping, sheet = "two_blank_rows", skip = 2)
  expect_identical(df, out)
})

test_that("leading blank rows can be implicitly AND explicitly skipped", {
  out <- read_excel(skipping, sheet = "two_blank_rows", skip = 1)
  expect_identical(df, out)
})

test_that("failure to skip junk leads to garbage df but no error", {
  out <- read_excel(skipping, sheet = "occupied_row_and_blank_row")
  expect_identical(out$`blah blah`, c(NA, "var1", "11", "21"))
  expect_identical(out[[2]], c(NA, "var2", "12", "22"))

  out <- read_excel(skipping, sheet = "two_occupied_rows")
  expect_identical(out$`blah blah`, c(NA, "var1", "11", "21"))
  expect_identical(out[[2]], c("yada yada", "var2", "12", "22"))

  out <- read_excel(skipping, sheet = "two_occupied_rows", skip = 1)
  expect_identical(out[[1]], c("var1", "11", "21"))
  expect_identical(out$`yada yada`, c("var2", "12", "22"))
})

test_that("explicit skip of leading junk, implicit skip of blank rows", {
  out <- read_excel(skipping, sheet = "occupied_row_and_blank_row", skip = 1)
  expect_identical(df, out)
})

test_that("explicit skip of leading junk and blank rows", {
  out <- read_excel(skipping, sheet = "occupied_row_and_blank_row", skip = 2)
  expect_identical(df, out)
})

test_that("explicit skip of leading junk", {
  out <- read_excel(skipping, sheet = "two_occupied_rows", skip = 2)
  expect_identical(out, df)
})

