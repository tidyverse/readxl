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
               col_types = rep_len(c("blank", "text"), length.out = 6)),
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

test_that("inappropriate col_types generate warning", {
  expect_warning(
    read_excel(test_sheet("iris-excel.xlsx"),
               col_types = c("numeric", "text", "numeric", "numeric", "numeric")),
    "Expecting numeric"
  )
  expect_warning(
    read_excel(test_sheet("iris-excel.xls"),
               col_types = c("numeric", "text", "numeric", "numeric", "numeric")),
    "Expecting numeric"
  )
})

test_that("types guessed correctly [xlsx]", {
  types <- read_excel(test_sheet("types.xlsx"), sheet = "guess_me")
  expect_is(types$X__1, "logical")
  expect_is(types$blank, "logical")
  expect_is(types$boolean, "logical")
  expect_is(types$date, "POSIXct")
  expect_is(types$numeric, "numeric")
  expect_is(types$text, "character")
  expect_true(all(vapply(types, function(x) is.na(x[3]), logical(1))))
})

test_that("types guessed correctly [xls]", {
  types <- read_excel(test_sheet("types.xls"), sheet = "guess_me")
  expect_is(types$X__1, "logical")
  expect_is(types$blank, "logical")
  expect_is(types$boolean, "logical")
  expect_is(types$date, "POSIXct")
  expect_is(types$numeric, "numeric")
  expect_is(types$text, "character")
  expect_true(all(vapply(types, function(x) is.na(x[3]), logical(1))))
})

test_that("guess_max is honored for col_types", {
  expect_warning(
    types <- read_excel(
      test_sheet("types.xlsx"),
      sheet = "guess_max",
      guess_max = 2
    ),
    "Expecting numeric"
  )
  expect_identical(types$string_in_row_3, c(1, 2, NA))
  expect_warning(
    types <- read_excel(
      test_sheet("types.xls"),
      sheet = "guess_max",
      guess_max = 2
    ),
    "Expecting numeric"
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
})

test_that("setting `na` works in list columns [xls]", {
  na_defined <-  read_excel(test_sheet("list_type.xls"), col_types = "list", na = "a")
  expect_equal(na_defined$var1[[3]], NA)
})

test_that("contaminated, explicit logical is read as logical", {
  ## xls
  expect_warning(
    df <- read_excel(test_sheet("types.xls"), sheet = "logical_coercion",
                     col_types = "logical"),
    "Expecting logical"
  )
  expect_is(df$logical, "logical")
  expect_false(anyNA(df$logical[c(1, 2, 4, 5, 7)]))

  ## xlsx
  expect_warning(
    df <- read_excel(test_sheet("types.xlsx"), sheet = "logical_coercion",
                     col_types = "logical"),
    "Expecting logical"
  )
  expect_is(df$logical, "logical")
  expect_false(anyNA(df$logical[c(1, 2, 4, 5, 7)]))
})

test_that("contaminated, explicit date is read as date", {
  ## xls
  expect_warning(
    df <- read_excel(test_sheet("types.xls"), sheet = "date_coercion",
                     col_types = "date"),
    "Expecting date"
  )
  expect_is(df$date, "POSIXct")
  expect_false(anyNA(df$date[c(1, 6, 7)]))

  ## xlsx
  expect_warning(
    df <- read_excel(test_sheet("types.xlsx"), sheet = "date_coercion",
                     col_types = "date"),
    "Expecting date"
  )
  expect_is(df$date, "POSIXct")
  expect_false(anyNA(df$date[c(1, 6, 7)]))
})

test_that("contaminated, explicit numeric is read as numeric", {
  ## xls
  expect_warning(
    df <- read_excel(test_sheet("types.xls"), sheet = "numeric_coercion",
                     col_types = "numeric"),
    "Expecting numeric|Coercing boolean|Coercing text"
  )
  expect_is(df$numeric, "numeric")
  expect_false(anyNA(df$numeric[c(1, 2, 4, 7)]))
  expect_equal(df$numeric[2], 72) # "Number stored as text"

  ## xlsx
  expect_warning(
    df <- read_excel(test_sheet("types.xlsx"), sheet = "numeric_coercion",
                     col_types = "numeric"),
    "Expecting numeric|Coercing boolean|Coercing text"
  )
  expect_is(df$numeric, "numeric")
  expect_false(anyNA(df$numeric[c(1, 2, 4, 7)]))
  expect_equal(df$numeric[2], 72) # "Number stored as text"
})

test_that("contaminated, explicit text is read as text", {
  ## xls
  df <- read_excel(test_sheet("types.xls"), sheet = "text_coercion",
                   col_types = "text")
  expect_is(df$text, "character")
  expect_false(anyNA(df$text[-2]))

  ## xlsx
  df <- read_excel(test_sheet("types.xlsx"), sheet = "text_coercion",
                   col_types = "text")
  expect_is(df$text, "character")
  expect_false(anyNA(df$text[-2]))
})
