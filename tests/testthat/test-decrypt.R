test_that("encrypted xlsx gives same data as its unencrypted equivalent", {
  expect_equal(
    read_xlsx(
      test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"),
      password = "cars"
    ),
    read_xlsx(test_sheet("mtcars-xlsx.xlsx"))
  )
})

test_that("Excelize encrypted xlsx fixtures can be read", {
  expect_equal(
    read_xlsx(
      test_sheet("encryptAES.xlsx"),
      password = "password",
      col_names = FALSE
    )[[1]],
    "SECRET"
  )
  expect_equal(
    read_xlsx(
      test_sheet("encryptSHA1.xlsx"),
      password = "password",
      col_names = FALSE
    )[[1]],
    "SECRET"
  )
  # EncryptedPackage payload is exactly 2 x 4096 bytes. This fixture was added
  # to Excelize in https://github.com/qax-os/excelize/pull/2329
  expect_equal(
    read_xlsx(
      test_sheet("encryptSHA512.xlsx"),
      password = "password",
      col_names = FALSE
    )[[1]],
    "SECRET"
  )
})

test_that("standard-encrypted xlsx does not print diagnostics", {
  expect_output(
    read_xlsx(
      test_sheet("encryptAES.xlsx"),
      password = "password",
      col_names = FALSE
    ),
    NA
  )
})

test_that("encrypted xlsx with a non-ASCII filename can be read", {
  skip_on_cran()
  tricky_filename <- "\u00C0\u00CB\u00D0-encrypted.xlsx"
  path <- file.path(tempdir(), tricky_filename)
  file.copy(
    test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"),
    path,
    overwrite = TRUE
  )
  on.exit(unlink(path))

  expect_equal(
    read_xlsx(path, password = "cars"),
    read_xlsx(test_sheet("mtcars-xlsx.xlsx"))
  )
})

test_that("excel_sheets() reads an encrypted xlsx", {
  expect_equal(
    excel_sheets(
      test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"),
      password = "cars"
    ),
    excel_sheets(test_sheet("mtcars-xlsx.xlsx"))
  )
})

test_that("ole_kind_() classifies files by their compound-file streams", {
  expect_equal(
    ole_kind_(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx")),
    "encrypted_xlsx"
  )
  expect_equal(ole_kind_(test_sheet("mtcars.xls")), "xls")
  expect_equal(ole_kind_(test_sheet("mtcars-xlsx.xlsx")), NA_character_)
})

test_that("excel_sheets() still reads an unencrypted xls", {
  expect_equal(excel_sheets(test_sheet("mtcars.xls")), "head(mtcars)")
})

test_that("read_excel() points to read_xlsx() for an encrypted xlsx", {
  expect_snapshot(
    error = TRUE,
    read_excel(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"))
  )
})

test_that("encrypted xlsx without a password errors", {
  rlang::local_interactive(FALSE)
  local_mocked_bindings(askpass_installed = \() TRUE)
  expect_snapshot(
    error = TRUE,
    read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"))
  )
})

test_that("encrypted xlsx with the wrong password errors", {
  expect_snapshot(
    error = TRUE,
    read_xlsx(
      test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"),
      password = "wrong"
    )
  )
})

test_that("a function-valued password is only called when needed", {
  called <- FALSE
  pw <- function() {
    called <<- TRUE
    "cars"
  }

  expect_no_error(read_xlsx(test_sheet("mtcars-xlsx.xlsx"), password = pw))
  expect_false(called)

  expect_equal(
    read_xlsx(
      test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"),
      password = pw
    ),
    read_xlsx(test_sheet("mtcars-xlsx.xlsx"))
  )
  expect_true(called)
})

test_that("a function-valued password that fails errors informatively", {
  expect_snapshot(
    error = TRUE,
    read_xlsx(
      test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"),
      password = function() stop("no password available")
    )
  )
})

test_that("password must be a single string or a function", {
  expect_snapshot(
    error = TRUE,
    read_xlsx(
      test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"),
      password = function() c("a", "b")
    )
  )
  expect_snapshot(
    error = TRUE,
    read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"), password = 42)
  )
})

test_that("encrypted xlsx prompts when interactive and askpass is installed", {
  rlang::local_interactive(TRUE)
  local_mocked_bindings(
    askpass_installed = function() TRUE,
    askpass_askpass = function() "cars"
  )
  expect_equal(
    read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx")),
    read_xlsx(test_sheet("mtcars-xlsx.xlsx"))
  )
})

test_that("encrypted xlsx with no password errors when prompting is impossible", {
  local_mocked_bindings(askpass_installed = function() FALSE)
  expect_snapshot(
    error = TRUE,
    read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"))
  )
})

test_that("a cancelled auto-prompt errors informatively", {
  rlang::local_interactive(TRUE)
  local_mocked_bindings(
    askpass_installed = function() TRUE,
    askpass_askpass = function() stop("Password prompt cancelled")
  )
  expect_snapshot(
    error = TRUE,
    read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"))
  )
})
