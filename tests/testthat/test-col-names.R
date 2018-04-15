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

test_that("wrong length column names are rejected", {
  err_msg <- "Sheet 1 has 5 columns (5 unskipped), but `col_names` has length 3."
  expect_error(
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), col_names = LETTERS[1:3]),
    err_msg,
    fixed = TRUE
  )
  expect_error(
    read_excel(test_sheet("iris-excel-xls.xls"), col_names = LETTERS[1:3]),
    err_msg,
    fixed = TRUE
  )
})

test_that("column_names can anticipate skipping", {

  ## xlsx
  expect_silent(
    df <- read_excel(
      test_sheet("iris-excel-xlsx.xlsx"),
      col_names = c("one", "two", "three"), skip = 1,
      col_types = c("numeric", "numeric", "skip", "skip", "text")
    )
  )
  expect_identical(dim(df), c(150L, 3L))
  expect_identical(names(df), c("one", "two", "three"))

  ## xls
  expect_silent(
    df <- read_excel(
      test_sheet("iris-excel-xls.xls"),
      col_names = c("one", "two", "three"), skip = 1,
      col_types = c("numeric", "numeric", "skip", "skip", "text")
    )
  )
  expect_identical(dim(df), c(150L, 3L))
  expect_identical(names(df), c("one", "two", "three"))
})
