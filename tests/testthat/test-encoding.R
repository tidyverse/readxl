context("Encoding")

test_that("excel_sheets returns utf-8 encoded text", {
  ## xlsx
  sheets <- excel_sheets(test_sheet("utf8-sheet-names.xlsx"))
  expect_identical(Encoding(sheets), rep("UTF-8", 2))
  expect_identical(sheets, c("\u00b5", "\u2202"))

  ## xls
  sheets <- excel_sheets(test_sheet("utf8-sheet-names.xls"))
  expect_identical(Encoding(sheets), rep("UTF-8", 2))
  expect_identical(sheets, c("\u00b5", "\u2202"))
})

test_that("read_excel returns utf-8 encoded text", {
  ## xlsx
  viet <- read_excel(test_sheet("vietnamese-utf8.xlsx"))
  nms <- names(viet)
  expect_identical(Encoding(nms), "UTF-8")
  expect_identical(
    charToRaw(nms),
    as.raw(c(0x4d, 0xc3, 0xb4, 0x20, 0x74, 0xe1, 0xba, 0xa3))
  )
  expect_identical(Encoding(viet[[1]]), rep_len("UTF-8", nrow(viet)))
  expect_identical(
    charToRaw(viet[[1]][1]),
    as.raw(c(0x42, 0xc3, 0x99, 0x4e, 0x20, 0x53, 0xc3, 0x89, 0x54))
  )

  ## xls
  viet <- read_excel(test_sheet("vietnamese-utf8.xls"))
  nms <- names(viet)
  expect_identical(Encoding(nms), "UTF-8")
  expect_identical(
    charToRaw(nms),
    as.raw(c(0x4d, 0xc3, 0xb4, 0x20, 0x74, 0xe1, 0xba, 0xa3))
  )
  expect_identical(Encoding(viet[[1]]), rep_len("UTF-8", nrow(viet)))
  expect_identical(
    charToRaw(viet[[1]][1]),
    as.raw(c(0x42, 0xc3, 0x99, 0x4e, 0x20, 0x53, 0xc3, 0x89, 0x54))
  )
})
