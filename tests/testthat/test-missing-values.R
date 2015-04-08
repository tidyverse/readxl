context("Missing values")

test_that("blanks read as missing [xlsx]", {
  blanks <- read_excel("blanks.xlsx")
  expect_equal(blanks$x, c(NA, 1))
  expect_equal(blanks$y, c("a", NA))
})

test_that("blanks read as missing [xls]", {
  blanks <- read_excel("blanks.xls")
  expect_equal(blanks$x, c(NA, 1))
  expect_equal(blanks$y, c("a", NA))
})

test_that("By default, NA read as text", {
  df <- read_xls("missing-values.xls")
  expect_equal(df$x, c("NA", "1.000000", "1.000000"))
})

test_that("na arg maps strings to to NA [xls]", {
  df <- read_excel("missing-values.xls", na = "NA")
  expect_equal(df$x, c(NA, 1, 1))
  expect_equal(df$y, c(NA, 1, 1)) # formula column
})

test_that("na arg maps strings to to NA [xlsx]", {
  df <- read_excel("missing-values.xlsx", na = "NA")
  expect_equal(df$x, c(NA, 1, 1))
  expect_equal(df$y, c(NA, 1, 1)) # formula column
})

test_that("text values in numeric column gives warning & NA", {
  expect_warning(
    df <- read_excel("missing-values.xls", col_types = rep("numeric", 2)),
    "Expecting numeric"
  )
  expect_equal(df$x, c(NA, 1, 1))
  expect_warning(
    df <- read_excel("missing-values.xlsx", col_types = rep("numeric", 2)),
    "expecting numeric"
  )
  expect_equal(df$x, c(NA, 1, 1))
})

test_that("empty first column gives valid data.frame", {
  df <- read_excel("missing-first-column.xlsx", col_names=F)
  expect_equal(nrow(df), length(df[,1]))
})

test_that("empty named column gives NA column", {
  df1 <- read_excel("empty-named-column.xlsx", col_names=T)
  df2 <- read_excel("empty-named-column.xls", col_names=T)
  expect_equal(ncol(df1), 4)
  expect_equal(names(df1)[2], "y")
  expect_equal(ncol(df2), 4)
  expect_equal(names(df2)[2], "y")
})
