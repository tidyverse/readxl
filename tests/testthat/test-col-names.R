context("Column names")

test_that("col_names = FALSE mimics missing column names [xlsx]", {
  df1 <- read_excel(
    test_sheet("no-yes-col-names.xlsx"),
    sheet = "no-col-names",
    col_names = FALSE
  )
  df2 <- read_excel(
    test_sheet("no-yes-col-names.xlsx"),
    sheet = "yes-col-names",
    skip = 1,
    col_names = c("", "")
  )
  df3 <- read_excel(
    test_sheet("no-yes-col-names.xlsx"),
    ## A1 and B1 aren't truly empty -- contain invisible single quotes, so we
    ## don't skip over them
    sheet = "empty-col-names"
  )
  expect_identical(df1, df2)
  expect_identical(df2, df3)
})

test_that("col_names = FALSE mimics missing column names [xls]", {
  df1 <- read_excel(
    test_sheet("no-yes-col-names.xls"),
    sheet = "no-col-names",
    col_names = FALSE
  )
  df2 <- read_excel(
    test_sheet("no-yes-col-names.xls"),
    sheet = "yes-col-names",
    skip = 1,
    col_names = c("", "")
  )
  df3 <- read_excel(
    test_sheet("no-yes-col-names.xls"),
    ## A1 and B1 aren't truly empty -- contain invisible single quotes, so we
    ## don't skip over them
    sheet = "empty-col-names"
  )
  expect_identical(df1, df2)
  expect_identical(df2, df3)
})

test_that("missing column names are populated", {
  df <- read_excel(test_sheet("unnamed-duplicated-columns.xlsx"))
  expect_identical(names(df)[c(1, 3)], c("X__1", "X__2"))
  df <- read_excel(test_sheet("unnamed-duplicated-columns.xls"))
  expect_identical(names(df)[c(1, 3)], c("X__1", "X__2"))
})

test_that("column names are de-duplicated", {
  df <- read_excel(test_sheet("unnamed-duplicated-columns.xlsx"))
  expect_identical(names(df)[4], "var2__1")
  df <- read_excel(test_sheet("unnamed-duplicated-columns.xls"))
  expect_identical(names(df)[4], "var2__1")
})
