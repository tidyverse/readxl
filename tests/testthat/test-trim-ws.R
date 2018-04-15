context("Trim whitespace")

test_that("trim_ws is default and it works", {
  ## data
  xls <- read_excel(test_sheet("whitespace-xls.xls"), col_names = FALSE)
  xlsx <- read_excel(test_sheet("whitespace-xlsx.xlsx"), col_names = FALSE)
  expect_identical(xls[[1]], trimws(xls[[1]]))
  expect_identical(xlsx[[1]], trimws(xlsx[[1]]))
  ## the 4th cell is "\t   \t"
  ## xls reports this but xlsx does not
  ## does xlsx return "" for cells containing only whitespace?
  expect_identical(xls[[1]], xlsx[[1]])

  ## column names
  xls <- read_excel(test_sheet("whitespace-xls.xls"))
  xlsx <- read_excel(test_sheet("whitespace-xlsx.xlsx"))
  expect_identical(names(xls), trimws(names(xls)))
  expect_identical(names(xlsx), trimws(names(xlsx)))
  expect_identical(names(xls), names(xlsx))
})

test_that("trim_ws = FALSE preserves whitespace", {
  ## data
  xls <- read_excel(
    test_sheet("whitespace-xls.xls"),
    col_names = FALSE, trim_ws = FALSE
  )
  xlsx <- read_excel(
    test_sheet("whitespace-xlsx.xlsx"),
    col_names = FALSE, trim_ws = FALSE
  )
  expect_false(identical(xls[[1]], trimws(xls[[1]])))
  expect_false(identical(xlsx[[1]], trimws(xlsx[[1]])))
  ## I have a whitespace-only cell with contents: "\t   \t"
  ## which survives import from xls, but not from xlsx
  ws_only <- grepl("^\\s*$", xls[[1]])
  expect_identical(xls[[1]][!ws_only], xlsx[[1]][!ws_only])

  ## column names
  xls <- read_excel(test_sheet("whitespace-xls.xls"), trim_ws = FALSE)
  xlsx <- read_excel(test_sheet("whitespace-xlsx.xlsx"), trim_ws = FALSE)
  expect_false(identical(names(xls), trimws(names(xls))))
  expect_false(identical(names(xlsx), trimws(names(xlsx))))
  expect_identical(names(xls), names(xlsx))
})

test_that("whitespace-flanked na strings match when trim_ws = TRUE", {
  xls <- read_excel(
    test_sheet("whitespace-xls.xls"),
    sheet = "logical_and_NA", na = ":-)"
  )
  xlsx <- read_excel(
    test_sheet("whitespace-xlsx.xlsx"),
    sheet = "logical_and_NA", na = ":-)"
  )
  expect_is(xls$numeric, "numeric")
  expect_is(xlsx$numeric, "numeric")
  expect_true(is.na(xls[[2, 1]]))
  expect_true(is.na(xlsx[[2, 1]]))
})

test_that("whitespace-flanked na strings do not match when trim_ws = FALSE", {
  xls <- read_excel(
    test_sheet("whitespace-xls.xls"),
    sheet = "logical_and_NA", na = ":-)", trim_ws = FALSE
  )
  xlsx <- read_excel(
    test_sheet("whitespace-xlsx.xlsx"),
    sheet = "logical_and_NA", na = ":-)", trim_ws = FALSE
  )
  expect_is(xls$numeric, "character")
  expect_is(xlsx$numeric, "character")
  expect_identical(xls[[2, 1]], "  :-)  ")
  expect_identical(xlsx[[2, 1]], "  :-)  ")
})
