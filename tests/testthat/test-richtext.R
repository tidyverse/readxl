context("Richtext")

test_that("rich text strings are handled in stringtable", {
  rt <- read_excel("richtext-coloured.xlsx", col_names = FALSE)

  for (i in 1:4)
    expect_equal(rt[1,i], "abcd")

  expect_equal(rt[2,1], "tvalrval1rval2")
  for (i in 2:4)
    expect_equal(rt[2,i], "rval1rval2")
})
