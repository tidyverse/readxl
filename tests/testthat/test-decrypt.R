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

test_that("a function-valued password is only called when needed", {
  called <- FALSE
  pw <- function() {
    called <<- TRUE
    "msoc"
  }
  expect_equal(
    read_excel(test_sheet("Encrypted.xlsx"), password = pw),
    read_excel(test_sheet("Untitled1.xlsx"))
  )
  expect_true(called)

  called <- FALSE
  expect_no_error(read_excel(test_sheet("Untitled1.xlsx"), password = pw))
  expect_false(called)
})

test_that("a function-valued password that fails errors informatively", {
  expect_snapshot(
    error = TRUE,
    read_excel(
      test_sheet("Encrypted.xlsx"),
      password = function() stop("no console")
    )
  )
})

test_that("password must be a single string or a function", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("Encrypted.xlsx"), password = function() c("a", "b"))
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("Encrypted.xlsx"), password = 42)
  )
})

test_that("encrypted xlsx prompts when interactive and askpass is installed", {
  local_mocked_bindings(
    is_interactive = function() TRUE,
    askpass_installed = function() TRUE,
    askpass_askpass = function(prompt) "msoc"
  )
  expect_equal(
    read_excel(test_sheet("Encrypted.xlsx")),
    read_excel(test_sheet("Untitled1.xlsx"))
  )
})

test_that("encrypted xlsx with no password errors when prompting is impossible", {
  local_mocked_bindings(askpass_installed = function() FALSE)
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("Encrypted.xlsx"))
  )
})

test_that("a cancelled auto-prompt errors informatively", {
  local_mocked_bindings(
    is_interactive = function() TRUE,
    askpass_installed = function() TRUE,
    askpass_askpass = function(prompt) stop("Password prompt cancelled")
  )
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("Encrypted.xlsx"))
  )
})
