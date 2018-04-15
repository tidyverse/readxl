context("coercion")

test_that("contaminated, explicit logical is read as logical", {
  ## xls
  expect_warning(
    df <- read_excel(
      test_sheet("types.xls"), sheet = "logical_coercion",
      col_types = c("logical", "text")
    ),
    "Expecting logical",
    all = TRUE
  )
  expect_is(df$logical, "logical")
  should_be_NA <- df$explanation %in% c("string not logical", "blank", "date")
  expect_false(anyNA(df$logical[!should_be_NA]))

  ## xlsx
  expect_warning(
    df <- read_excel(
      test_sheet("types.xlsx"), sheet = "logical_coercion",
      col_types = c("logical", "text")
    ),
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
    df <- read_excel(
      test_sheet("types.xls"), sheet = "date_coercion",
      col_types = "date"
    ),
    "Expecting date|Coercing numeric",
    all = TRUE
  )
  expect_is(df$date, "POSIXct")
  expect_false(anyNA(df$date[c(1, 5, 6, 7)]))
  expect_true(all(is.na(df$date[c(2, 3, 4)])))
  expect_identical(df$date[6], as.POSIXct("2012-01-02 UTC", tz = "UTC"))

  ## xlsx
  expect_warning(
    df <- read_excel(
      test_sheet("types.xlsx"), sheet = "date_coercion",
      col_types = "date"
    ),
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
    df <- read_excel(
      test_sheet("types.xls"), sheet = "numeric_coercion",
      col_types = "numeric"
    ),
    "Expecting numeric|Coercing boolean|Coercing text",
    all = TRUE
  )
  expect_is(df$numeric, "numeric")
  expect_false(anyNA(df$numeric[c(1, 2, 4, 7)]))
  expect_equal(df$numeric[2], 72) # "Number stored as text"

  ## xlsx
  expect_warning(
    df <- read_excel(
      test_sheet("types.xlsx"), sheet = "numeric_coercion",
      col_types = "numeric"
    ),
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
  df <- read_excel(
    test_sheet("types.xls"), sheet = "text_coercion",
    col_types = c("text", "text")
  )
  expect_is(df$text, "character")
  expect_false(anyNA(df$explanation != "blank"))
  expect_identical(df$text[df$explanation == "floating point"], "1.3")
  expect_identical(df$text[df$explanation == "student number"], "36436153")

  ## xlsx
  df <- read_excel(
    test_sheet("types.xlsx"), sheet = "text_coercion",
    col_types = c("text", "text")
  )
  expect_is(df$text, "character")
  expect_false(anyNA(df$explanation != "blank"))
  expect_identical(df$text[df$explanation == "floating point"], "1.3")
  expect_identical(df$text[df$explanation == "student number"], "36436153")
})

test_that("integery-y numbers > 2^31 can be coerced to string", {
  xlsx <- read_excel(test_sheet("big-texty-numbers-xlsx.xlsx"))
  xls <- read_excel(test_sheet("big-texty-numbers-xls.xls"))
  expect_identical(xlsx, xls)
  expect_identical(
    xls[["number"]][-1],
    as.character(
      c(2 ^ 31 - 1, 2 ^ 31, 2 ^ 31 + 1, -1 * (2 ^ 31), -1 * (2 ^ 31 + 1))
    )
  )
})

## https://github.com/tidyverse/readxl/issues/430
## re: precision of datetimes converted to text
test_that("datetimes agree up to certain precision when coerced to text", {
  xlsx <- read_excel(test_sheet("texty-dates-xlsx.xlsx"), col_types = "text")
  xls <- read_excel(test_sheet("texty-dates-xls.xls"), col_types = "text")

  xlsx <- sub("^.*\\.(.*)$", "\\1", xlsx$a)
  xls <- sub("^.*\\.(.*)$", "\\1", xls$a)
  ## it's hard to say how many digits of agreement we expect after the decimal
  ## we coerce to string for xls, whereas its out of our hands for xlsx,
  ## where the double comes to us as a string in the XML
  ## therefore, my choice is somewhat arbitrary
  xlsx <- substr(xlsx, 1, 8)
  xls <- substr(xls, 1, 8)

  expect_identical(xlsx, xls)
})
