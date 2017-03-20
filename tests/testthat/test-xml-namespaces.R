context("XML namespaces")

## #268, #202, #80
## what is special about nonstandard-xml-ns-prefix.xlsx?
## note `ns:id="rId3"` vs `r:id="rId3"`
## `ns:id` has been seen in xlsx written by 3rd party tools
## `r:id` is typical of files written by Excel
# <workbook xmlns="http://schemas.openxmlformats.org/spreadsheetml/2006/main">
#   <bookViews>
#   <workbookView/>
#   </bookViews>
#   <sheets>
#   <sheet xmlns:ns="http://schemas.openxmlformats.org/officeDocument/2006/relationships" name="Sheet1" sheetId="1" ns:id="rId3"/>
#   </sheets>
#   <definedNames/>
# </workbook>

test_that("XML namespace prefixes are stripped", {
  df <- read_excel(test_sheet("nonstandard-xml-ns-prefix.xlsx"))
  exp <- tibble::tibble(
    a = c(1, 2),
    b = c(3, 4)
  )
  expect_identical(df, exp)
})
