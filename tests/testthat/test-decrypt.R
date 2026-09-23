test_that("encrypted xlsx round-trips to its plaintext equivalent", {
  expect_equal(
    read_excel(test_sheet("Encrypted.xlsx"), password = "msoc"),
    read_excel(test_sheet("Untitled1.xlsx"))
  )
  expect_equal(
    read_xlsx(test_sheet("Encrypted.xlsx"), password = "msoc"),
    read_xlsx(test_sheet("Untitled1.xlsx"))
  )
})

test_that("excel_sheets() reads an encrypted xlsx", {
  expect_equal(
    excel_sheets(test_sheet("Encrypted.xlsx"), password = "msoc"),
    excel_sheets(test_sheet("Untitled1.xlsx"))
  )
})

test_that("encrypted xlsx without a password errors", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("Encrypted.xlsx"))
  )
})

test_that("encrypted xlsx with the wrong password errors", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("Encrypted.xlsx"), password = "wrong")
  )
})

test_that("password on a non-encrypted file errors", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("Untitled1.xlsx"), password = "msoc")
  )
  expect_snapshot(
    error = TRUE,
    read_xls(test_sheet("iris-excel-xls.xls"), password = "msoc")
  )
})
