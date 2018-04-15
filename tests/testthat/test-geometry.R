context("Sheet geometry")

## testing of the unexported standardise_limits() ---------------------

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
  l2 <- standardise_limits(
    range = NULL, skip = sample(1:100, 1),
    n_max = 0, has_col_names = FALSE
  )
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

test_that("we can request n rows for data via n_max and skip doesn't matter", {
  my_skip <- sample(0:100, 1)
  n <- sample(1:100, 1)
  l <- standardise_limits(range = NULL, skip = my_skip, n_max = n, has_col_names = TRUE)
  expect_identical(l, limitize(c(my_skip, my_skip + n, -1, -1)))

  my_skip <- sample(0:100, 1)
  n <- sample(1:100, 1)
  l <- standardise_limits(range = NULL, skip = my_skip, n_max = n, has_col_names = FALSE)
  expect_identical(l, limitize(c(my_skip, my_skip + n - 1, -1, -1)))
})

## testing via read_excel() -------------------------------------------

test_that("geometry sheet is identical in xls and xlsx and shrink wraps by default", {
  ref <- tibble::tibble(
    var1 = c("aa", "dd", "gg"),
    var2 = c("bb", "ee", "hh"),
    var3 = c("cc", "ff", "ii")
  )
  xls <- read_excel(test_sheet("geometry.xls"))
  xlsx <- read_excel(test_sheet("geometry.xls"))
  expect_identical(xls, xlsx)
  expect_identical(xls, ref)
})

## at time of writing, clippy/geometry.key contains useful sketches
## for choosing test cases
## they are not comprehensive, but cover a very large number of scenarios

test_that("exact range request for data rectangle works", {
  ref <- tibble::tibble(
    var1 = c("aa", "dd", "gg"),
    var2 = c("bb", "ee", "hh"),
    var3 = c("cc", "ff", "ii")
  )
  xls <- read_excel(test_sheet("geometry.xls"), range = "B3:D6")
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = "B3:D6")
  expect_identical(xls, xlsx)
  expect_identical(xls, ref)
})

test_that("range request for proper subset of data rectangle works", {
  ref <- tibble::tibble(
    var1 = c("aa"),
    var2 = c("bb")
  )
  xls <- read_excel(test_sheet("geometry.xls"), range = "B3:C4")
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = "B3:C4")
  expect_identical(xls, xlsx)
  expect_identical(xls, ref)
})

test_that("range requests that run off one or more edges of data rectangle work", {
  ## all four edges
  xls <- read_excel(test_sheet("geometry.xls"), range = "A2:E7")
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = "A2:E7")
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(5L, 5L))

  ## top edge only, partial
  xls <- read_excel(test_sheet("geometry.xls"), range = "B2:C3")
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = "B2:C3")
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(1L, 2L))

  ## top and right, partial
  xls <- read_excel(test_sheet("geometry.xls"), range = "D2:E3")
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = "D2:E3")
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(1L, 2L))

  ## right edge only, partial
  xls <- read_excel(test_sheet("geometry.xls"), range = "C4:E5")
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = "C4:E5")
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(1L, 3L))

  ## left and right edges, partial
  xls <- read_excel(test_sheet("geometry.xls"), range = "A4:E5")
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = "A4:E5")
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(1L, 5L))

  ## left and bottom edges, partial
  xls <- read_excel(test_sheet("geometry.xls"), range = "A6:B7")
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = "A6:B7")
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(1L, 2L))
})

test_that("requests that use cell_rows() only work", {
  ## rows are within the data
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_rows(2:3))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_rows(2:3))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(1L, 3L))

  ## rows run past the data
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_rows(3:7))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_rows(3:7))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(4L, 3L))

  ## rows start before the data
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_rows(1:4))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_rows(1:4))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(3L, 3L))

  ## rows start before and run past the data
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_rows(1:7))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_rows(1:7))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(6L, 3L))

  ## unspecified upper row bound
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_rows(c(3, NA)))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_rows(c(3, NA)))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(3L, 3L))
  ## unspecified lower bound not relevant because cellranger handles
})

test_that("requests that use cell_cols() only work", {
  ## cols are within the data
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_cols(2:3))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_cols(2:3))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(3L, 2L))

  ## cols run past the data
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_cols(2:5))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_cols(2:5))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(3L, 4L))

  ## cols start before the data
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_cols(1:3))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_cols(1:3))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(3L, 3L))

  ## cols start before and run past the data
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_cols(1:5))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_cols(1:5))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(3L, 5L))

  ## unspecified upper col bound
  xls <- read_excel(test_sheet("geometry.xls"), range = cell_cols(c(3, NA)))
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = cell_cols(c(3, NA)))
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(3L, 2L))
  ## unspecified lower bound not relevant because cellranger handles
})

test_that("open rectangles work", {
  ## only worth testing openness on right and bottom
  ## cellranger handles openness on left and top

  ## open on bottom
  lims <- cell_limits(c(5, 3), c(NA, 5))
  xls <- read_excel(test_sheet("geometry.xls"), range = lims)
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = lims)
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(1L, 3L))

  ## open on right
  lims <- cell_limits(c(3, 3), c(5, NA))
  xls <- read_excel(test_sheet("geometry.xls"), range = lims)
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = lims)
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(2L, 2L))

  ## open on bottom and right
  lims <- cell_limits(c(4, 3), c(NA, NA))
  xls <- read_excel(test_sheet("geometry.xls"), range = lims)
  xlsx <- read_excel(test_sheet("geometry.xlsx"), range = lims)
  expect_identical(xls, xlsx)
  expect_identical(dim(xls), c(2L, 2L))
})
