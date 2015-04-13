context("Richtext")

test_that("rich text strings are handled in stringtable", {
  rt <- read_excel("richtext-coloured.xlsx", col_names = FALSE)

  for (i in 1:4)
    expect_equal(rt[1,i], "abcd")

  expect_equal(rt[2,1], "tvalrval1rval2")
  for (i in 2:4)
    expect_equal(rt[2,i], "rval1rval2")
})

test_that("rich text inside inlineStr", {
  # Original source: http://our.componentone.com/wp-content/uploads/2011/12/TestExcel.xlsx
  # Modified to have Excel-safe mixed use of <r> and <t>
  x <- read_excel("inlineStr2.xlsx")
  expect_equal(x$ID, "RQ11610")
  expect_equal(names(x)[1], "NNNN")
  expect_equal(names(x)[2], "BeforeHierarchy")
})
