context("Column types")

test_that("illegal col_types are rejected", {
  expect_error(
    read_excel(
      test_sheet("types.xlsx"),
      col_types = c("foo", "numeric", "text", "bar")
    ),
    "Illegal column type"
  )
})

test_that("request for 'blank' col type gets deprecation message and fix", {
  expect_message(
    read_excel(
      test_sheet("types.xlsx"),
      col_types = rep_len(c("blank", "text"), length.out = 5)
    ),
    "`col_type = \"blank\"` deprecated. Use \"skip\" instead.",
    fixed = TRUE
  )
})

test_that("invalid col_types are rejected", {
  expect_error(
    read_excel(test_sheet("types.xlsx"), col_types = character()),
    "length(col_types) > 0 is not TRUE", fixed = TRUE
  )
  expect_error(
    read_excel(test_sheet("types.xlsx"), col_types = 1:3),
    "is.character(col_types) is not TRUE", fixed = TRUE
  )
  expect_error(
    read_excel(test_sheet("types.xlsx"), col_types = c(NA, "text", "numeric")),
    "!anyNA(col_types) is not TRUE", fixed = TRUE
  )
})

test_that("col_types can be specified", {
  df <- read_excel(
    test_sheet("iris-excel-xlsx.xlsx"),
    col_types = c("numeric", "text", "numeric", "numeric", "text")
  )
  expect_is(df[[2]], "character")
  df <- read_excel(
    test_sheet("iris-excel-xls.xls"),
    col_types = c("numeric", "text", "numeric", "numeric", "text")
  )
  expect_is(df[[2]], "character")
})

test_that("col_types are recycled", {
  df <- read_excel(test_sheet("iris-excel-xlsx.xlsx"), col_types = "text")
  expect_match(vapply(df, class, character(1)), "character")
  df <- read_excel(test_sheet("iris-excel-xls.xls"), col_types = "text")
  expect_match(vapply(df, class, character(1)), "character")
})

test_that("types guessed correctly [xlsx]", {
  types <- read_excel(test_sheet("types.xlsx"), sheet = "guess_me")
  expect_is(types$blank, "logical")
  expect_is(types$boolean, "logical")
  expect_is(types$date, "POSIXct")
  expect_is(types$numeric, "numeric")
  expect_is(types$text, "character")
  expect_true(all(vapply(types, function(x) is.na(x[3]), logical(1))))
})

test_that("types guessed correctly [xls]", {
  types <- read_excel(test_sheet("types.xls"), sheet = "guess_me")
  expect_is(types$blank, "logical")
  expect_is(types$boolean, "logical")
  expect_is(types$date, "POSIXct")
  expect_is(types$numeric, "numeric")
  expect_is(types$text, "character")
  expect_true(all(vapply(types, function(x) is.na(x[3]), logical(1))))
})

test_that("we can specify some col_types and guess others", {
  ctypes <- c("text", "guess", "guess", "text", "guess")
  exp_cls <- c("character", "logical", "POSIXct", "character", "character")

  df <- read_excel(test_sheet("types.xlsx"), col_types = ctypes)
  cls <- vapply(df, function(x) class(x)[1], character(1))
  expect_equivalent(cls, exp_cls)

  df <- read_excel(test_sheet("types.xls"), col_types = ctypes)
  cls <- vapply(df, function(x) class(x)[1], character(1))
  expect_equivalent(cls, exp_cls)
})

test_that("guess_max is honored for col_types", {
  expect_warning(
    types <- read_excel(
      test_sheet("types.xlsx"),
      sheet = "guess_max",
      guess_max = 2
    ),
    "Expecting numeric",
    all = TRUE
  )
  expect_identical(types$string_in_row_3, c(1, 2, NA))
  expect_warning(
    types <- read_excel(
      test_sheet("types.xls"),
      sheet = "guess_max",
      guess_max = 2
    ),
    "Expecting numeric",
    all = TRUE
  )
  expect_identical(types$string_in_row_3, c(1, 2, NA))
})

test_that("wrong length col types generates error", {
  err_msg <- "Sheet 1 has 5 columns, but `col_types` has length 2."
  expect_error(
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), col_types = c("numeric", "text")),
    err_msg
  )
  expect_error(
    read_excel(test_sheet("iris-excel-xls.xls"), col_types = c("numeric", "text")),
    err_msg
  )
})

test_that("list column reads data correctly [xlsx]", {
  types <- read_excel(test_sheet("list_type.xlsx"), col_types = "list")
  expect_equal(types$var1[[1]], 1)
  expect_equal(types$var1[[2]], NA)
  expect_equal(types$var1[[3]], "a")
  expect_equal(types$var1[[4]], as.POSIXct("2017-01-01", tz = "UTC"))
  expect_equal(types$var1[[5]], "abc")
  expect_equal(types$var1[[6]], FALSE)
})

test_that("setting `na` works in list columns [xlsx]", {
  na_defined <- read_excel(test_sheet("list_type.xlsx"), col_types = "list", na = "a")
  expect_equal(na_defined$var1[[3]], NA)
})

test_that("list column reads data correctly [xls]", {
  types <- read_excel(test_sheet("list_type.xls"), col_types = "list")
  expect_equal(types$var1[[1]], 1)
  expect_equal(types$var1[[2]], NA)
  expect_equal(types$var1[[3]], "a")
  expect_equal(types$var1[[4]], as.POSIXct("2017-01-01", tz = "UTC"))
  expect_equal(types$var1[[5]], "abc")
  expect_equal(types$var1[[6]], FALSE)
})

test_that("setting `na` works in list columns [xls]", {
  na_defined <- read_excel(test_sheet("list_type.xls"), col_types = "list", na = "a")
  expect_equal(na_defined$var1[[3]], NA)
})

## #385: logical-guessed column could end up with multiple pathological `TRUE`
## values for subsequent cells holding numeric data
## refines fix initiated in #398
test_that("numeric is correctly coerced to logical [xlsx]", {
  expect_warning(
    df <- read_xlsx(test_sheet("missing-values-xlsx.xlsx"), guess_max = 0)
  )
  expect_identical(df$z, c(NA, TRUE, TRUE))
  expect_equal(sum(df$z, na.rm = TRUE), 2)
})

test_that("numeric is correctly coerced to logical [xls]", {
  expect_warning(
    df <- read_xls(test_sheet("missing-values-xls.xls"), guess_max = 0)
  )
  expect_identical(df$z, c(NA, TRUE, TRUE))
  expect_equal(sum(df$z, na.rm = TRUE), 2)
})
