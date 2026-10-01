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

test_that("a fully specified range over empty cells returns blank cells", {
  for (ext in c("xlsx", "xls")) {
    path <- test_sheet(paste0("empty-sheets.", ext))

    out <- read_excel(path, "empty", range = "A1:C1", col_names = FALSE)
    expect_identical(
      out,
      tibble::tibble(...1 = NA, ...2 = NA, ...3 = NA)
    )

    out <- read_excel(path, "empty", range = "D14", col_names = FALSE)
    expect_identical(out, tibble::tibble(...1 = NA))

    out <- read_excel(path, "empty", range = "B2:C4", col_names = c("a", "b"))
    expect_identical(out, tibble::tibble(a = rep(NA, 3), b = rep(NA, 3)))

    out <- read_excel(path, "empty", range = "A1:C1")
    expect_identical(
      out,
      tibble::tibble(...1 = logical(), ...2 = logical(), ...3 = logical())
    )
  }
})

test_that("a range over empty cells in a non-empty sheet returns blank cells", {
  for (ext in c("xlsx", "xls")) {
    path <- test_sheet(paste0("skipping.", ext))
    out <- read_excel(path, range = "A100:B102", col_names = FALSE)
    expect_identical(out, tibble::tibble(...1 = rep(NA, 3), ...2 = rep(NA, 3)))
  }
})

test_that("an open-ended range over empty cells still returns nothing", {
  path <- test_sheet("empty-sheets.xlsx")
  expect_identical(
    read_excel(path, "empty", range = cellranger::cell_rows(1:3)),
    tibble::tibble()
  )
  expect_identical(
    read_excel(path, "empty", range = cellranger::cell_cols("B:D")),
    tibble::tibble()
  )
})
