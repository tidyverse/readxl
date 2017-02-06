context("Sheets")

test_that("excel_sheets returns utf-8 encoded text", {
  sheets <- excel_sheets(test_sheet("utf8-sheets.xlsx"))
  expect_equal(Encoding(sheets), rep("UTF-8", 2))
  expect_equal(sheets, c("\u00b5", "\u2202"))

  # Equivalent code with xls fails for reasons that I don't understand.
  # maybe libxls isn't reencording? It's a bit suspicious that mu is
  # \u00b5 and libxls is giving \xb5
})

test_that("informative error when requesting non-existent sheet by name", {
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), sheet = "tulip"),
    "Sheet 'tulip' not found"
  )
})

test_that("informative error when requesting non-existent sheet by position", {
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), sheet = 200),
    "Can't retrieve sheet in position"
  )
})

test_that("invalid sheet values caught", {
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), sheet = 0),
    "`sheet` must be positive"
  )
  expect_error(
    read_excel(test_sheet("iris-excel.xlsx"), sheet = rep(1L, 2)),
    "`sheet` must have length 1"
  )
})

test_that("sheet data xml target is explicitly looked up (#104, #80)", {
  xlsx <- test_sheet("sheet-xml-lookup.xlsx")
  countries <- excel_sheets(xlsx)
  ## what's important about this workbook?
  ## xml target for worksheet i is NOT sheeti.xml
  ##       name    Id                   Target
  ##      <chr> <chr>                    <chr>
  ## 1   Africa  rId3 xl/worksheets/sheet4.xml
  ## 2 Americas  rId4 xl/worksheets/sheet3.xml
  ## 3     Asia  rId5 xl/worksheets/sheet5.xml
  ## 4   Europe  rId6 xl/worksheets/sheet1.xml
  ## 5  Oceania  rId7 xl/worksheets/sheet2.xml
  ## tests that we find xml target like so: (name ->) i -> Id -> Target
  ## embedded-chartsheet.xlsx has an embedded chartsheet but I see no
  ## current reason to explicitly test (#116, #200)
  for (cty in countries) {
    df <- read_excel(xlsx, sheet = cty)
    expect_identical(df$continent[1], cty)
  }
  for (i in seq_along(countries)) {
    df <- read_excel(xlsx, sheet = i)
    expect_identical(df$continent[1], countries[i])
  }
})

test_that("missing column names are populated", {
  df <- read_excel(test_sheet("unnamed-duplicated-columns.xlsx"))
  expect_identical(names(df)[c(1, 3)], c("V1", "V2"))
  df <- read_excel(test_sheet("unnamed-duplicated-columns.xls"))
  expect_identical(names(df)[c(1, 3)], c("V1", "V2"))
})

test_that("column names are de-duplicated", {
  df <- read_excel(test_sheet("unnamed-duplicated-columns.xlsx"))
  expect_identical(names(df)[4], "var21")
  df <- read_excel(test_sheet("unnamed-duplicated-columns.xls"))
  expect_identical(names(df)[4], "var21")
})
