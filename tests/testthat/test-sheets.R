context("Sheets")

test_that("excel_sheets returns utf-8 encoded text", {
  sheets <- excel_sheets("utf8-sheets.xlsx")
  expect_equal(Encoding(sheets), rep("UTF-8", 2))
  expect_equal(sheets, c("\u00b5", "\u2202"))

  # Equivalent code with xls fails for reasons that I don't understand.
  # maybe libxls isn't reencording? It's a bit suspicious that mu is
  # \u00b5 and libxls is giving \xb5
})
