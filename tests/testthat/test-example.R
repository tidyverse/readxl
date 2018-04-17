context("readxl_example")

test_that("NULL path returns names of example files", {
  example_files <- readxl_example()
  expect_true("datasets.xls" %in% example_files)
  expect_true("datasets.xlsx" %in% example_files)
  expect_is(example_files, "character")
})

test_that("providing example file name returns full path", {
  expect_true(file.exists(readxl_example("datasets.xlsx")))
})
