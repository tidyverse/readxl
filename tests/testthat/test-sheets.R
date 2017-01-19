context("Sheets")

test_that("excel_sheets returns utf-8 encoded text", {
  sheets <- excel_sheets("utf8-sheets.xlsx")
  expect_equal(Encoding(sheets), rep("UTF-8", 2))
  expect_equal(sheets, c("\u00b5", "\u2202"))

  # Equivalent code with xls fails for reasons that I don't understand.
  # maybe libxls isn't reencording? It's a bit suspicious that mu is
  # \u00b5 and libxls is giving \xb5
})

## #104, #168, some of #80
## #116, #200 (chartsheet case)
test_that("sheet data xml target is explicitly looked up [xlsx]", {
  xlsx <- "sheet-xml-lookup.xlsx"
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
  ## tests that we find xml target like so: name -> Id -> Target
  for (cty in countries) {
    df <- read_excel(xlsx, sheet = cty)
    expect_identical(df$continent[1], cty)
  }
  for (i in seq_along(countries)) {
    df <- read_excel(xlsx, sheet = i)
    expect_identical(df$continent[1], countries[i])
  }
})
