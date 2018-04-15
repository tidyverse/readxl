context("Richtext")

test_that("rich text strings are handled in stringtable", {
  rt <- read_excel(test_sheet("richtext-coloured.xlsx"), col_names = FALSE)

  for (i in 1:4)
    expect_equal(rt[[1, i]], "abcd")

  expect_equal(rt[[2, 1]], "tvalrval1rval2")
  for (i in 2:4)
    expect_equal(rt[[2, i]], "rval1rval2")
})

test_that("rich text inside inlineStr", {
  # Original source: http://our.componentone.com/wp-content/uploads/2011/12/TestExcel.xlsx
  # Modified to have Excel-safe mixed use of <r> and <t>
  x <- read_excel(test_sheet("inlineStr2.xlsx"))
  expect_equal(x$ID, "RQ11610")
  expect_equal(names(x)[1], "NNNN")
  expect_equal(names(x)[2], "BeforeHierarchy")
})

test_that("strings containing escaped hexcodes are read", {
  df <- read_excel(test_sheet("new_line_errors.xlsx"))
  expect_false(grepl("_x000D_", df[1, 1]))
  expect_equal(substring(df[1, 1], 20, 21), "\u000d\r")
  expect_equal(substring(df[2, 1], 11, 19), "\"_x000D_\"")
})
