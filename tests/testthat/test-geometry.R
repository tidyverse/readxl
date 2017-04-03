context("Sheet geometry")

limitize <- function(x) {
  stopifnot(length(x) == 4)
  setNames(x, c("min_row", "max_row", "min_col", "max_col"))
}

test_that("we can say 'read nothing' via n_max and col_names", {
  ## n_max = 0 and col_names is not TRUE --> read nothing
  l <- standardise_limits(range = NULL, skip = 0, n_max = 0, has_col_names = FALSE)
  expect_identical(l, limitize(c(-2, -1, -1, -1)))
  ## min_row max_row min_col max_col
  ##      -2      -1      -1      -1

  ## it should not matter what skip is
  l2 <- standardise_limits(range = NULL, skip = sample(1:100, 1),
                           n_max = 0, has_col_names = FALSE)
  expect_identical(l, l2)
})

test_that("we can request exactly 1 row for col_names via n_max and col_names", {
  my_skip <- sample(0:100, 1)
  l <- standardise_limits(range = NULL, skip = my_skip, n_max = 0, has_col_names = TRUE)
  expect_identical(l, limitize(c(my_skip, my_skip, -1, -1)))
})

test_that("we can request exactly 1 row for data via n_max and col_names", {
  my_skip <- sample(0:100, 1)
  l <- standardise_limits(range = NULL, skip = my_skip, n_max = 1, has_col_names = FALSE)
  expect_identical(l, limitize(c(my_skip, my_skip, -1, -1)))
})

test_that("we can request n rows for data via n_max", {
  my_skip <- sample(0:100, 1)
  n <- sample(1:100, 1)
  l <- standardise_limits(range = NULL, skip = my_skip, n_max = n, has_col_names = TRUE)
  expect_identical(l, limitize(c(my_skip, my_skip + n, -1, -1)))

  my_skip <- sample(0:100, 1)
  n <- sample(1:100, 1)
  l <- standardise_limits(range = NULL, skip = my_skip, n_max = n, has_col_names = FALSE)
  expect_identical(l, limitize(c(my_skip, my_skip + n - 1, -1, -1)))
})

test_that("range controls geometry: limit rows only or cols only", {
  df <- read_excel(test_sheet("geometry.xls"), range = cell_rows(2:3))
  expect_identical(dim(df), c(1L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_rows(3:7))
  expect_identical(dim(df), c(4L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols(2:4))
  expect_identical(dim(df), c(3L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols(c(NA, 3)))
  expect_identical(dim(df), c(3L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols(c(1, NA)))
  expect_identical(dim(df), c(3L, 4L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols(c(2, NA)))
  expect_identical(dim(df), c(3L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols(c(3, NA)))
  expect_identical(dim(df), c(3L, 2L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols(c(4, NA)))
  expect_identical(dim(df), c(3L, 1L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols("B"))
  expect_identical(dim(df), c(3L, 1L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols("B:C"))
  expect_identical(dim(df), c(3L, 2L))
  df <- read_excel(test_sheet("geometry.xls"), range = cell_cols(LETTERS))
  expect_identical(dim(df), c(3L, 26L))
})

test_that("range controls geometry: proper rectangles", {
  df <- read_excel(test_sheet("geometry.xls"), range = "B3:C5")
  expect_identical(dim(df), c(2L, 2L))
  df <- read_excel(test_sheet("geometry.xls"), range = "C3:D7")
  expect_identical(dim(df), c(4L, 2L))
  df <- read_excel(test_sheet("geometry.xls"), range = "C3:E5")
  expect_identical(dim(df), c(2L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = "C1:D4")
  expect_identical(dim(df), c(3L, 2L))
  df <- read_excel(test_sheet("geometry.xls"), range = "A2:C3")
  expect_identical(dim(df), c(1L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = "A4:C5")
  expect_identical(dim(df), c(1L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = "A6:C7")
  expect_identical(dim(df), c(1L, 3L))
  df <- read_excel(test_sheet("geometry.xls"), range = "A4:E5")
  expect_identical(dim(df), c(1L, 5L))
})
