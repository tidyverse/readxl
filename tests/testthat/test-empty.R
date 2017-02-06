context("Empty sheets")

test_that("completely empty sheets are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", skip = 3)
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", col_names = "a")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", col_names = FALSE)
  expect_identical(out, tibble::tibble())
})

test_that("sheets with column names only are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "header_only")
  expect_identical(out, tibble::tibble(var1 = numeric(), var2 = numeric()))
})

test_that("non-empty sheets act that way if we skip past everything", {
  ## currently we get a 0-row tibble with 2 unnamed columns
  ## a big improvement but when I rationlize column names, see why these
  ## empty column names aren't causing the columns to be dropped and fix it
  out <- read_excel(test_sheet("skipping.xlsx"), skip = 10)
  expect_is(out, "tbl_df")
  expect_identical(nrow(out), 0L)
  skip("figure out why empty unnamed columns aren't dropped and update test")
})

test_that("empty cells with a style are not loaded", {
  ## what's important about this sheet?
  ## contains empty cells with a custom format
  ## therefore they appear in the xml and have a style attribute
  ## where are they?
  ## in the embedded empty columns, w/ and w/o a name
  ## in a trailing empty column
  ## in some trailing rows
  out <- read_excel(test_sheet("style-only-cells.xlsx"))
  df <- tibble::tibble(var1 = c("val1,1", "val2,1", "val3,1"),
                       var2 = NA_real_,
                       var3 = c("aa", "bb", "cc"),
                       var5 = c(1, 2, 3))
  expect_equal(out, df)
  skip("revisit this when dust settles re: treatment of empty columns")
})

test_that("user-supplied column names play nicely with empty columns", {
  skip("waiting for dust to settle re: treatment of empty columns")
  ## do stuff like this:
  out <- read_excel(
    test_sheet("style-only-cells.xlsx"),
    col_names = LETTERS[1:4]
  )
  out <- read_excel(
    test_sheet("style-only-cells.xlsx"),
    col_names = LETTERS[1:5]
  )
})
