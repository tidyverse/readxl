context("Sheets")

test_that("informative error when requesting non-existent sheet by name", {
  expect_error(
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = "tulip"),
    "Sheet 'tulip' not found"
  )
  expect_error(
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), range = "tulip!A1:A1"),
    "Sheet 'tulip' not found"
  )
})

test_that("informative error when requesting non-existent sheet by position", {
  expect_error(
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = 2),
    "Can't retrieve sheet in position 2, only 1 sheet(s) found.",
    fixed = TRUE
  )
  expect_error(
    read_excel(test_sheet("iris-excel-xls.xls"), sheet = 2),
    "Can't retrieve sheet in position 2, only 1 sheet(s) found.",
    fixed = TRUE
  )
})

test_that("invalid sheet values caught", {
  expect_error(
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = 0),
    "`sheet` must be positive"
  )
  expect_error(
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = rep(1L, 2)),
    "`sheet` must have length 1"
  )
})

test_that("sheet can be parsed out of range", {
  direct <-
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = "iris", range = "A1:A1")
  indirect <-
    read_excel(test_sheet("iris-excel-xlsx.xlsx"), range = "iris!A1:A1")
  expect_identical(direct, indirect)
})

test_that("double specification of sheet generates message and range wins", {
  expect_message(
    double <-
      read_excel(
        test_sheet("sheet-xml-lookup.xlsx"),
        sheet = "Asia", range = "Oceania!A1:A1"
      ),
    "Two values given for `sheet`. Using the `sheet` found in `range`"
  )
  ref <- read_excel(test_sheet("sheet-xml-lookup.xlsx"), range = "Oceania!A1:A1")
  expect_identical(double, ref)
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
