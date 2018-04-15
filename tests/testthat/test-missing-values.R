context("Missing values")

test_that("blanks in different rows read as missing [xlsx]", {
  blanks <- read_excel(test_sheet("blanks.xlsx"), sheet = "different_rows")
  expect_equal(blanks$x, c(NA, 1))
  expect_equal(blanks$y, c("a", NA))
})

test_that("blanks read as missing [xls]", {
  blanks <- read_excel(test_sheet("blanks.xls"), sheet = "different_rows")
  expect_equal(blanks$x, c(NA, 1))
  expect_equal(blanks$y, c("a", NA))
})

test_that("blanks in same middle row are read as missing [xlsx]", {
  blanks <- read_excel(test_sheet("blanks.xlsx"), sheet = "same_row_middle")
  expect_equal(blanks$x, c(1, NA, 2))
  expect_equal(blanks$y, c("a", NA, "b"))
})

test_that("blanks in same middle row are read as missing [xls]", {
  blanks <- read_excel(test_sheet("blanks.xls"), sheet = "same_row_middle")
  expect_equal(blanks$x, c(1, NA, 2))
  expect_equal(blanks$y, c("a", NA, "b"))
})

test_that("blanks in same, first row are read as missing [xlsx]", {
  blanks <- read_excel(test_sheet("blanks.xlsx"), sheet = "same_row_first")
  expect_equal(blanks$x, c(NA, 1))
  expect_equal(blanks$y, c(NA, "a"))
})

test_that("blanks in same, first row are read as missing [xls]", {
  blanks <- read_excel(test_sheet("blanks.xls"), sheet = "same_row_first")
  expect_equal(blanks$x, c(NA, 1))
  expect_equal(blanks$y, c(NA, "a"))
})

test_that("By default, NA read as text", {
  df <- read_xls(test_sheet("missing-values-xls.xls"))
  expect_equal(df$x, c("NA", "1", "2"))
})

test_that("na arg can be used to map 'NA' to NA [xls]", {
  df <- read_excel(test_sheet("missing-values-xls.xls"), na = "NA")
  expect_equal(df$x, c(NA, 1, 2))
  expect_equal(df$y, c(NA, 1, 2)) # formula column
})

test_that("na arg can be used to map 'NA' to NA [xlsx]", {
  df <- read_excel(test_sheet("missing-values-xlsx.xlsx"), na = "NA")
  expect_equal(df$x, c(NA, 1, 2))
  expect_equal(df$y, c(NA, 1, 2)) # formula column
})

## #401 make sure check against NA strings is done after shared string table
## lookup for affected cells vs. on the index into the shared string table
test_that("na arg works with multiple strings and for shared strings [xlsx]", {
  df <- read_excel(
    test_sheet("numbers-as-na-and-shared-strings-xlsx.xlsx"),
    na = c("1", "d")
  )
  ## shared string table is:
  ## sst[0] = "a"
  ## sst[1] = "b" <-- make sure this does not become NA!
  ## sst[2] = "c"
  ## sst[3] = "d"
  expect_identical(df, tibble::tribble(
    ~ a, ~ c,
    "b", NA
  ))
})

test_that("na arg works with multiple strings and for shared strings [xls]", {
  df <- read_excel(
    test_sheet("numbers-as-na-and-shared-strings-xls.xls"),
    na = c("1", "d")
  )
  expect_identical(df, tibble::tribble(
    ~ a, ~ c,
    "b", NA
  ))
})

test_that("empty first column gives valid data.frame [xls]", {
  df <- read_excel(test_sheet("missing-first-column.xls"), col_names = FALSE)
  expect_s3_class(df, "tbl_df")
  expect_identical(dim(df), c(3L, 1L))
})

test_that("empty first column gives valid data.frame [xlsx]", {
  df <- read_excel(test_sheet("missing-first-column.xlsx"), col_names = FALSE)
  expect_s3_class(df, "tbl_df")
  expect_identical(dim(df), c(3L, 1L))
})

test_that("empty named column gives NA column", {
  df1 <- read_excel(test_sheet("empty-named-column.xlsx"), col_names = TRUE)
  df2 <- read_excel(test_sheet("empty-named-column.xls"), col_names = TRUE)
  expect_equal(ncol(df1), 4)
  expect_equal(names(df1)[2], "y")
  expect_true(all(is.na(df1$y)))
  expect_true(all(is.logical(df1$y)))
  expect_equal(ncol(df2), 4)
  expect_equal(names(df2)[2], "y")
  expect_true(all(is.na(df2$y)))
  expect_true(all(is.logical(df2$y)))
})

test_that("empty (styled) cells are not loaded, but can survive as NA [xlsx]", {
  ## what's important about this sheet?
  ## contains empty cells with a custom format
  ## therefore they appear in the xml and have a style attribute
  ## where are they?
  ## in the embedded empty columns, w/ and w/o a name
  ## in a trailing empty column WHICH SHOULD BE DROPPED
  ## in some trailing rows WHICH SHOULD BE DROPPED
  out <- read_excel(test_sheet("style-only-cells.xlsx"))
  df <- tibble::tibble(
    var1 = c("val1,1", "val2,1", "val3,1"),
    var2 = NA,
    var3 = c("aa", "bb", "cc"),
    X__1 = NA,
    var5 = c(1, 2, 3)
  )
  expect_equal(out, df)
})

test_that("empty (styled) cells are not loaded, but can survive as NA [xls]", {
  out <- read_excel(test_sheet("style-only-cells.xls"))
  df <- tibble::tibble(
    var1 = c("val1,1", "val2,1", "val3,1"),
    var2 = NA,
    var3 = c("aa", "bb", "cc"),
    X__1 = NA,
    var5 = c(1, 2, 3)
  )
  expect_equal(out, df)
})
