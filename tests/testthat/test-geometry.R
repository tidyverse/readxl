context("Sheet geometry")

limitize <- function(x) {
  stopifnot(length(x) == 4)
  setNames(x, c("min_row", "max_row", "min_col", "max_col"))
}

test_that("we can say 'read nothing' via n_max and col_names", {
  ## n_max = 0 and col_names is not TRUE --> read nothing
  l <- standardise_limits(range = NULL, skip = 0, n_max = 0, has_col_names = FALSE)
  expect_identical(l, limitize(rep(-1, 4)))
  ## min_row max_row min_col max_col
  ##      -1      -1      -1      -1

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
