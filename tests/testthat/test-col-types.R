context("Column types")

test_that("illegal col_types are rejected", {
  expect_error(
    read_excel(test_sheet("types.xlsx"),
               col_types = c("foo", "numeric", "text", "bar")),
    "Illegal column type"
  )
})

test_that("request for 'blank' col type gets deprecation message and fix", {
  expect_message(
    read_excel(test_sheet("types.xlsx"),
               col_types = rep_len(c("blank", "text"), length.out = 5)),
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
  df <- read_excel(test_sheet("iris-excel.xlsx"),
                   col_types = c("numeric", "text", "numeric", "numeric", "text"))
  expect_is(df[[2]], "character")
  df <- read_excel(test_sheet("iris-excel.xls"),
                   col_types = c("numeric", "text", "numeric", "numeric", "text"))
  expect_is(df[[2]], "character")
})

test_that("col_types are recycled", {
  df <- read_excel(test_sheet("iris-excel.xlsx"), col_types = "text")
  expect_match(vapply(df, class, character(1)), "character")
  df <- read_excel(test_sheet("iris-excel.xls"), col_types = "text")
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
    read_excel(test_sheet("iris-excel.xlsx"), col_types = c("numeric", "text")),
    err_msg
  )
  expect_error(
    read_excel(test_sheet("iris-excel.xls"), col_types = c("numeric", "text")),
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
  na_defined <-  read_excel(test_sheet("list_type.xlsx"), col_types = "list", na = "a")
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
  na_defined <-  read_excel(test_sheet("list_type.xls"), col_types = "list", na = "a")
  expect_equal(na_defined$var1[[3]], NA)
})

test_that("contaminated, explicit logical is read as logical", {
  ## xls
  expect_warning(
    df <- read_excel(test_sheet("types.xls"), sheet = "logical_coercion",
                     col_types = c("logical", "text")),
    "Expecting logical",
    all = TRUE
  )
  expect_is(df$logical, "logical")
  should_be_NA <- df$explanation %in% c("string not logical", "blank", "date")
  expect_false(anyNA(df$logical[!should_be_NA]))

  ## xlsx
  expect_warning(
    df <- read_excel(test_sheet("types.xlsx"), sheet = "logical_coercion",
                     col_types = c("logical", "text")),
    "Expecting logical",
    all = TRUE
  )
  expect_is(df$logical, "logical")
  should_be_NA <- df$explanation %in% c("string not logical", "blank", "date")
  expect_false(anyNA(df$logical[!should_be_NA]))
})

test_that("contaminated, explicit date is read as date", {
  ## xls
  expect_warning(
    df <- read_excel(test_sheet("types.xls"), sheet = "date_coercion",
                     col_types = "date"),
    "Expecting date|Coercing numeric",
    all = TRUE
  )
  expect_is(df$date, "POSIXct")
  expect_false(anyNA(df$date[c(1, 5, 6, 7)]))
  expect_true(all(is.na(df$date[c(2, 3, 4)])))
  expect_identical(df$date[6], as.POSIXct("2012-01-02 UTC", tz = "UTC"))

  ## xlsx
  expect_warning(
    df <- read_excel(test_sheet("types.xlsx"), sheet = "date_coercion",
                     col_types = "date"),
    "Expecting date|Coercing numeric",
    all = TRUE
  )
  expect_is(df$date, "POSIXct")
  expect_false(anyNA(df$date[c(1, 5, 6, 7)]))
  expect_true(all(is.na(df$date[c(2, 3, 4)])))
  expect_identical(df$date[6], as.POSIXct("2012-01-02 UTC", tz = "UTC"))
})

test_that("contaminated, explicit numeric is read as numeric", {
  ## xls
  expect_warning(
    df <- read_excel(test_sheet("types.xls"), sheet = "numeric_coercion",
                     col_types = "numeric"),
    "Expecting numeric|Coercing boolean|Coercing text",
    all = TRUE
  )
  expect_is(df$numeric, "numeric")
  expect_false(anyNA(df$numeric[c(1, 2, 4, 7)]))
  expect_equal(df$numeric[2], 72) # "Number stored as text"

  ## xlsx
  expect_warning(
    df <- read_excel(test_sheet("types.xlsx"), sheet = "numeric_coercion",
                     col_types = "numeric"),
    "Expecting numeric|Coercing boolean|Coercing text",
    all = TRUE
  )
  expect_is(df$numeric, "numeric")
  expect_false(anyNA(df$numeric[c(1, 2, 4, 7)]))
  expect_equal(df$numeric[2], 72) # "Number stored as text"
})

## #75, #110: don't convert numbers to string a la "printf %lf"
## i.e. don't right pad to get 6 decimal places
test_that("contaminated, explicit text is read as text", {
  ## xls
  df <- read_excel(test_sheet("types.xls"), sheet = "text_coercion",
                   col_types = c("text", "text"))
  expect_is(df$text, "character")
  expect_false(anyNA(df$explanation != "blank"))
  expect_identical(df$text[df$explanation == "floating point"], "1.3")
  expect_identical(df$text[df$explanation == "student number"], "36436153")

  ## xlsx
  df <- read_excel(test_sheet("types.xlsx"), sheet = "text_coercion",
                   col_types = c("text", "text"))
  expect_is(df$text, "character")
  expect_false(anyNA(df$explanation != "blank"))
  expect_identical(df$text[df$explanation == "floating point"], "1.3")
  expect_identical(df$text[df$explanation == "student number"], "36436153")

})

## #385: transform non-zero integers to 1 before coercing an integer column to
## logical so that different 'kinds' of TRUE aren't returned, which cause
## segfaults in some base R functions.
test_that("integers other than 1 are converted to the same kind of TRUE", {
  x <- read_xlsx(test_sheet("int-to-logical.xlsx"), guess_max = 0)
  expect_equal(x$x, c(NA, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE))
  expect_error(table(x$x), NA)
})
