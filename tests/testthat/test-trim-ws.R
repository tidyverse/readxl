context("Trim whitespace")

test_that("trim_ws is default and it works", {
  ## data
  xls <- read_excel(test_sheet("whitespace.xls"), col_names = FALSE)
  xlsx <- read_excel(test_sheet("whitespace.xlsx"), col_names = FALSE)
  expect_identical(xls[[1]], trimws(xls[[1]]))
  expect_identical(xlsx[[1]], trimws(xlsx[[1]]))
  ## the 4th cell is "\t   \t"
  ## xls reports this but xlsx does not
  ## does xlsx return "" for cells containing only whitespace?
  expect_identical(xls[[1]], xlsx[[1]])

  ## column names
  xls <- read_excel(test_sheet("whitespace.xls"))
  xlsx <- read_excel(test_sheet("whitespace.xlsx"))
  expect_identical(names(xls), trimws(names(xls)))
  expect_identical(names(xlsx), trimws(names(xlsx)))
  expect_identical(names(xls), names(xlsx))
})

test_that("trim_ws = FALSE preserves whitespace", {
  ## data
  xls <- read_excel(test_sheet("whitespace.xls"),
                    col_names = FALSE, trim_ws = FALSE)
  xlsx <- read_excel(test_sheet("whitespace.xlsx"),
                     col_names = FALSE, trim_ws = FALSE)
  expect_false(identical(xls[[1]], trimws(xls[[1]])))
  expect_false(identical(xlsx[[1]], trimws(xlsx[[1]])))
  ## I have a whitespace-only cell with contents: "\t   \t"
  ## which survives import from xls, but not from xlsx
  ws_only <- grepl("^\\s*$", xls[[1]])
  expect_identical(xls[[1]][!ws_only], xlsx[[1]][!ws_only])

  ## column names
  xls <- read_excel(test_sheet("whitespace.xls"), trim_ws = FALSE)
  xlsx <- read_excel(test_sheet("whitespace.xlsx"), trim_ws = FALSE)
  expect_false(identical(names(xls), trimws(names(xls))))
  expect_false(identical(names(xlsx), trimws(names(xlsx))))
  expect_identical(names(xls), names(xlsx))
})
