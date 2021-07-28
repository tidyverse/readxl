test_that("NULL path returns names of example files", {
  example_files <- readxl_example()
  expect_true("datasets.xls" %in% example_files)
  expect_true("datasets.xlsx" %in% example_files)
  expect_true(is.character(example_files))
})

test_that("providing example file name returns full path", {
  expect_true(file.exists(readxl_example("datasets.xlsx")))
})
