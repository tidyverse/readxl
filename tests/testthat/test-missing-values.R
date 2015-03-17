context("Missing values")

test_that("By default, NA read as text", {
  df <- read_xls("missing-values.xls")
  expect_equal(df$x, c("NA", "1.000000", "1.000000"))
})

test_that("na arg maps strings to to NA", {
  df <- read_xls("missing-values.xls", na = "NA")
  expect_equal(df$x, c(NA, 1, 1))
})

test_that("text values in numeric column gives warning & NA", {
  expect_warning(
    df <- read_xls("missing-values.xls", col_types = "numeric"),
    "Expecting numeric"
  )
  expect_equal(df$x, c(NA, 1, 1))
})
