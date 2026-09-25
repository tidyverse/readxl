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

test_that("excel_sheets() does not probe encryption for actual xls files", {
  local_mocked_bindings(
    is_encrypted_xlsx_ = function(path) stop("unexpected encryption probe")
  )
  expect_equal(excel_sheets(test_sheet("mtcars.xls")), "head(mtcars)")
})

test_that("encrypted xlsx without a password errors", {
  rlang::local_interactive(FALSE)
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
