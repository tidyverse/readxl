test_that("completely empty sheets are handled [xlsx]", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", skip = 3)
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", col_names = "a")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xlsx"), "empty", col_names = FALSE)
  expect_identical(out, tibble::tibble())
})

test_that("completely empty sheets are handled [xls]", {
  out <- read_excel(test_sheet("empty-sheets.xls"), "empty")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xls"), "empty", skip = 3)
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xls"), "empty", col_names = "a")
  expect_identical(out, tibble::tibble())

  out <- read_excel(test_sheet("empty-sheets.xls"), "empty", col_names = FALSE)
  expect_identical(out, tibble::tibble())
})

test_that("sheets with column names only are handled", {
  out <- read_excel(test_sheet("empty-sheets.xlsx"), "header_only")
  expect_identical(out, tibble::tibble(var1 = logical(), var2 = logical()))
  out <- read_excel(test_sheet("empty-sheets.xls"), "header_only")
  expect_identical(out, tibble::tibble(var1 = logical(), var2 = logical()))
})

test_that("non-empty sheets act that way if we skip past everything", {
  out <- read_excel(test_sheet("skipping.xlsx"), skip = 10)
  expect_identical(out, tibble::tibble())
  out <- read_excel(test_sheet("skipping.xls"), skip = 10)
  expect_identical(out, tibble::tibble())
})

test_that("a fully specified range over empty cells is honored, col_names = TRUE", {
  xls <- test_sheet("empty-sheets.xls")
  xlsx <- test_sheet("empty-sheets.xlsx")

  ref <- tibble::tibble(...1 = logical(), ...2 = logical(), ...3 = logical())
  out_xls <- read_excel(xls, "empty", range = "A1:C1")
  out_xlsx <- read_excel(xlsx, "empty", range = "A1:C1")
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)

  ref <- tibble::tibble(...1 = logical())
  out_xls <- read_excel(xls, "empty", range = "D14")
  out_xlsx <- read_excel(xlsx, "empty", range = "D14")
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)

  ref <- tibble::tibble(...1 = rep(NA, 2), ...2 = rep(NA, 2))
  out_xls <- read_excel(xls, "empty", range = "B2:C4")
  out_xlsx <- read_excel(xlsx, "empty", range = "B2:C4")
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)
})

test_that("a fully specified range over empty cells is honored, col_names = FALSE", {
  xls <- test_sheet("empty-sheets.xls")
  xlsx <- test_sheet("empty-sheets.xlsx")

  ref <- tibble::tibble(...1 = NA, ...2 = NA, ...3 = NA)
  out_xls <- read_excel(xls, "empty", range = "A1:C1", col_names = FALSE)
  out_xlsx <- read_excel(xlsx, "empty", range = "A1:C1", col_names = FALSE)
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)

  ref <- tibble::tibble(...1 = NA)
  out_xls <- read_excel(xls, "empty", range = "D14", col_names = FALSE)
  out_xlsx <- read_excel(xlsx, "empty", range = "D14", col_names = FALSE)
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)

  ref <- tibble::tibble(...1 = rep(NA, 3), ...2 = rep(NA, 3))
  out_xls <- read_excel(xls, "empty", range = "B2:C4", col_names = FALSE)
  out_xlsx <- read_excel(xlsx, "empty", range = "B2:C4", col_names = FALSE)
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)
})

test_that("a fully specified range over empty cells is honored, col_names = <chr>", {
  xls <- test_sheet("empty-sheets.xls")
  xlsx <- test_sheet("empty-sheets.xlsx")

  ref <- tibble::tibble(a = NA, b = NA, c = NA)
  nms <- c("a", "b", "c")
  out_xls <- read_excel(xls, "empty", range = "A1:C1", col_names = nms)
  out_xlsx <- read_excel(xlsx, "empty", range = "A1:C1", col_names = nms)
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)

  ref <- tibble::tibble(a = NA)
  out_xls <- read_excel(xls, "empty", range = "D14", col_names = "a")
  out_xlsx <- read_excel(xlsx, "empty", range = "D14", col_names = "a")
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)

  ref <- tibble::tibble(a = rep(NA, 3), b = rep(NA, 3))
  nms <- c("a", "b")
  out_xls <- read_excel(xls, "empty", range = "B2:C4", col_names = nms)
  out_xlsx <- read_excel(xlsx, "empty", range = "B2:C4", col_names = nms)
  expect_identical(out_xls, out_xlsx)
  expect_identical(out_xls, ref)
})

test_that("a range over empty cells in a non-empty sheet returns blank cells", {
  ref <- tibble::tibble(...1 = rep(NA, 3), ...2 = rep(NA, 3))
  xls <- read_excel(
    test_sheet("skipping.xls"),
    range = "A100:B102",
    col_names = FALSE
  )
  xlsx <- read_excel(
    test_sheet("skipping.xlsx"),
    range = "A100:B102",
    col_names = FALSE
  )
  expect_identical(xls, xlsx)
  expect_identical(xls, ref)
})

test_that("an open-ended range over empty cells doesn't error", {
  # TO THINK ON: I haven't really decided what such ranges should return
  # and don't want to settle it right now
  # I imagine cell_cols("B:D") should return 0 rows x 3 cols, though
  xlsx <- test_sheet("empty-sheets.xlsx")
  expect_no_error(
    read_excel(xlsx, "empty", range = cellranger::cell_rows(1:3))
  )
  expect_no_error(
    read_excel(xlsx, "empty", range = cellranger::cell_cols("B:D"))
  )
})
