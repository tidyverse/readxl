context("n_max")

test_that("simple use of n_max works and does not affect col name reading", {
  ## xlsx
  df <- read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = 1)
  expect_identical(nrow(df), 1L)
  df <- read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = 0)
  expect_identical(nrow(df), 0L)
  expect_identical(ncol(df), 5L)
  expect_identical(names(df), names(iris))

  ## xls
  df <- read_excel(test_sheet("iris-excel-xls.xls"), n_max = 1)
  expect_identical(nrow(df), 1L)
  df <- read_excel(test_sheet("iris-excel-xls.xls"), n_max = 0)
  expect_identical(nrow(df), 0L)
  expect_identical(ncol(df), 5L)
  expect_identical(names(df), names(iris))
})

test_that("n_max = 0 and col_names = FALSE gives empty tibble", {
  ## xlsx
  df <- read_excel(
    test_sheet("skipping.xlsx"), sheet = "two_occupied_rows",
    n_max = 0, col_names = FALSE
  )
  expect_identical(df, tibble::tibble())

  ## xls
  df <- read_excel(
    test_sheet("skipping.xls"), sheet = "two_occupied_rows",
    n_max = 0, col_names = FALSE
  )
  expect_identical(df, tibble::tibble())
})

test_that("n_max is upper bound on nrows, if it causes trailing blank row", {
  ## xlsx
  df <- read_excel(
    test_sheet("skipping.xlsx"), sheet = "two_occupied_rows",
    n_max = 3
  )
  expect_identical(nrow(df), 2L)

  ## xls
  df <- read_excel(
    test_sheet("skipping.xls"), sheet = "two_occupied_rows",
    n_max = 3
  )
  expect_identical(nrow(df), 2L)
})

test_that("n_max can affect ncols, if prevents read of data in a col [xlsx]", {
  ## xlsx
  df <- read_excel(
    test_sheet("skipping.xlsx"), sheet = "two_occupied_rows",
    n_max = 0
  )
  expect_identical(nrow(df), 0L)
  expect_identical(ncol(df), 1L)
  df <- read_excel(
    test_sheet("skipping.xlsx"), sheet = "two_occupied_rows",
    skip = 1, n_max = 0
  )
  expect_identical(nrow(df), 0L)
  expect_identical(ncol(df), 1L)
})

test_that("n_max can affect ncols, if prevents read of data in a col [xls]", {
  ## xls
  df <- read_excel(
    test_sheet("skipping.xls"), sheet = "two_occupied_rows",
    n_max = 0
  )
  expect_identical(nrow(df), 0L)
  expect_identical(ncol(df), 1L)
  df <- read_excel(
    test_sheet("skipping.xls"), sheet = "two_occupied_rows",
    skip = 1, n_max = 0
  )
  expect_identical(nrow(df), 0L)
  expect_identical(ncol(df), 1L)
})

test_that("n_max = nrows in dense sheet when col_names = FALSE", {
  ## xlsx
  df <- read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = 18, col_names = FALSE)
  expect_identical(nrow(df), 18L)

  ## xls
  df <- read_excel(test_sheet("iris-excel-xls.xls"), n_max = 18, col_names = FALSE)
  expect_identical(nrow(df), 18L)
})

test_that("n_max directive survives implicit skipping of empty rows [xlsx]", {
  ## col_names = TRUE
  explicit <-
    read_excel(test_sheet("geometry.xlsx"), skip = 2, n_max = 1)
  implicit_skip_all <-
    read_excel(test_sheet("geometry.xlsx"), n_max = 1)
  mixed_skip <-
    read_excel(test_sheet("geometry.xlsx"), skip = 1, n_max = 1)
  expect_identical(explicit, implicit_skip_all)
  expect_identical(explicit, mixed_skip)

  ## col_names = FALSE
  explicit <-
    read_excel(test_sheet("geometry.xlsx"), skip = 2, n_max = 1, col_names = FALSE)
  implicit_skip_all <-
    read_excel(test_sheet("geometry.xlsx"), n_max = 1, col_names = FALSE)
  mixed_skip <-
    read_excel(test_sheet("geometry.xlsx"), skip = 1, n_max = 1, col_names = FALSE)
  expect_identical(explicit, implicit_skip_all)
  expect_identical(explicit, mixed_skip)
})

test_that("n_max directive survives implicit skipping of empty rows [xls]", {
  ## col_names = TRUE
  explicit <-
    read_excel(test_sheet("geometry.xls"), skip = 2, n_max = 1)
  implicit_skip_all <-
    read_excel(test_sheet("geometry.xls"), n_max = 1)
  mixed_skip <-
    read_excel(test_sheet("geometry.xls"), skip = 1, n_max = 1)
  expect_identical(explicit, implicit_skip_all)
  expect_identical(explicit, mixed_skip)

  ## col_names = FALSE
  explicit <-
    read_excel(test_sheet("geometry.xls"), skip = 2, n_max = 1, col_names = FALSE)
  implicit_skip_all <-
    read_excel(test_sheet("geometry.xls"), n_max = 1, col_names = FALSE)
  mixed_skip <-
    read_excel(test_sheet("geometry.xls"), skip = 1, n_max = 1, col_names = FALSE)
  expect_identical(explicit, implicit_skip_all)
  expect_identical(explicit, mixed_skip)
})
