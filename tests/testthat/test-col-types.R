context("Column types")

test_that("illegal col_types are rejected", {
  expect_error(
    read_excel(test_sheet("types.xlsx"),
               col_types = c("foo", "numeric", "text", "bar")),
    "Illegal column type"
  )
})

test_that("col_types can be specified", {
  df <- read_excel(test_sheet("iris-excel.xlsx"),
                   col_types = c("numeric", "text", "numeric", "numeric", "text"))
  expect_is(df[[2]], "character")
})

test_that("inappropriate col_types generate warning", {
  expect_warning(
    read_excel(test_sheet("iris-excel.xlsx"),
               col_types = c("numeric", "text", "numeric", "numeric", "numeric")),
    "expecting numeric"
  )
})

test_that("types imputed & read correctly [xlsx]", {
  types <- read_excel(test_sheet("types.xlsx"))
  expect_is(types$number, "numeric")
  expect_is(types$string, "character")
  expect_is(types$boolean, "numeric")
  expect_is(types$date, "POSIXct")
  expect_is(types$string_in_row_3, "character")
  skip("switch expecation to logical (vs numeric) when possible")
})

test_that("types imputed & read correctly [xls]", {
  expect_output(
    types <- read_excel(test_sheet("types.xls")),
    "Unknown type: 517"
  )
  expect_is(types$number, "numeric")
  expect_is(types$string, "character")
  #expect_is(types$boolean, "numeric")
  #expect_is(types$date, "POSIXct")
  expect_is(types$string_in_row_3, "character")
  skip("revisit these expectations as xls problems are fixed")
})

test_that("max_guess is honored for col_types [xlsx]", {
  expect_warning(
    types <- read_excel(test_sheet("types.xlsx"), guess_max = 2),
    "expecting numeric"
  )
  expect_identical(types$string_in_row_3, c(1, 2, NA))
})

test_that("max_guess is honored for col_types [xls]", {
  skip("write this test as xls problems are fixed")
})
