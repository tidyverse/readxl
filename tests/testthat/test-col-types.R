test_that("illegal col_types are rejected", {
  expect_snapshot(
    error = TRUE,
    read_excel(
      test_sheet("types.xlsx"),
      col_types = c("foo", "numeric", "text", "bar")
    )
  )
})

test_that("request for 'blank' col type gets deprecation message and fix", {
  expect_snapshot(
    read_excel(
      test_sheet("types.xlsx"),
      col_types = rep_len(c("blank", "text"), length.out = 5)
    )
  )
})

test_that("invalid col_types are rejected", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("types.xlsx"), col_types = character()),
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("types.xlsx"), col_types = 1:3)
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("types.xlsx"), col_types = c(NA, "text", "numeric")),
  )
})

test_that("col_types can be specified", {
  df <- read_excel(
    test_sheet("iris-excel-xlsx.xlsx"),
    col_types = c("numeric", "text", "numeric", "numeric", "text")
  )
  expect_true(is.character(df[[2]]))
  df <- read_excel(
    test_sheet("iris-excel-xls.xls"),
    col_types = c("numeric", "text", "numeric", "numeric", "text")
  )
  expect_true(is.character(df[[2]]))
})

test_that("col_types are recycled", {
  df <- read_excel(test_sheet("iris-excel-xlsx.xlsx"), col_types = "text")
  expect_match(vapply(df, class, character(1)), "character")
  df <- read_excel(test_sheet("iris-excel-xls.xls"), col_types = "text")
  expect_match(vapply(df, class, character(1)), "character")
})

test_that("types guessed correctly [xlsx]", {
  types <- read_excel(test_sheet("types.xlsx"), sheet = "guess_me")
  expect_true(is.logical(types$blank))
  expect_true(is.logical(types$boolean))
  expect_s3_class(types$date, "POSIXct")
  expect_true(is.numeric(types$numeric))
  expect_true(is.character(types$text))
  expect_true(all(vapply(types, function(x) is.na(x[3]), logical(1))))
})

test_that("types guessed correctly [xls]", {
  types <- read_excel(test_sheet("types.xls"), sheet = "guess_me")
  expect_true(is.logical(types$blank))
  expect_true(is.logical(types$boolean))
  expect_s3_class(types$date, "POSIXct")
  expect_true(is.numeric(types$numeric))
  expect_true(is.character(types$text))
  expect_true(all(vapply(types, function(x) is.na(x[3]), logical(1))))
})

test_that("we can specify some col_types and guess others", {
  ctypes <- c("text", "guess", "guess", "text", "guess")
  exp_cls <- c("character", "logical", "POSIXct", "character", "character")

  df <- read_excel(test_sheet("types.xlsx"), col_types = ctypes)
  cls <- vapply(df, function(x) class(x)[1], character(1))
  expect_equal(cls, exp_cls, ignore_attr = TRUE)

  df <- read_excel(test_sheet("types.xls"), col_types = ctypes)
  cls <- vapply(df, function(x) class(x)[1], character(1))
  expect_equal(cls, exp_cls, ignore_attr = TRUE)
})

test_that("guess_max is honored for col_types", {
  expect_snapshot(
    types <- read_excel(
      test_sheet("types.xlsx"),
      sheet = "guess_max",
      guess_max = 2
    )
  )

  expect_identical(types$string_in_row_3, c(1, 2, NA))

  expect_snapshot(
    types <- read_excel(
      test_sheet("types.xls"),
      sheet = "guess_max",
      guess_max = 2
    )
  )

  expect_identical(types$string_in_row_3, c(1, 2, NA))
})

test_that("wrong length col types generates error", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), col_types = c("numeric", "text"))
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("iris-excel-xls.xls"), col_types = c("numeric", "text"))
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
  expect_snapshot(
    df <- read_xlsx(test_sheet("missing-values-xlsx.xlsx"),
      guess_max = 0
    )
  )

  expect_identical(df$z, c(NA, TRUE, TRUE))
  expect_equal(sum(df$z, na.rm = TRUE), 2)
})

test_that("numeric is correctly coerced to logical [xls]", {
  expect_snapshot(
    df <- read_xls(test_sheet("missing-values-xls.xls"),
      guess_max = 0
    )
  )

  expect_identical(df$z, c(NA, TRUE, TRUE))
  expect_equal(sum(df$z, na.rm = TRUE), 2)
})
