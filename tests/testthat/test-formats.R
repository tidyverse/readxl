context("test-formats.R")

test_that("excel_format() recognizes extensions (or lack thereof)", {
  files <- c("a.xlsx", "b.xls", "c.png", "d")
  expect_identical(
    format_from_ext(files),
    c("xlsx", "xls", NA, NA)
  )
})

test_that("format_from_signature() works for xls and xlsx and not jpg", {
  orig_files <- c(
    test_sheet("iris-excel-xlsx.xlsx"),
    test_sheet("iris-excel-xls.xls"),
    file.path(R.home("doc"), "html", "logo.jpg")
  )
  no_ext_copies <- paste0(
    basename(tools::file_path_sans_ext(orig_files)),
    "-no-ending"
  )
  no_ext_copies <- file.path(tempdir(), no_ext_copies)
  file.copy(orig_files, no_ext_copies)
  on.exit(file.remove(no_ext_copies))
  expect_identical(
    format_from_signature(no_ext_copies),
    c("xlsx", "xls", NA)
  )
})

test_that("excel_format(guess = FALSE) == format_from_ext()", {
  files <- c("a.xlsx", "b.xls", "c.png", "d")
  expect_identical(format_from_ext(files), excel_format(files))
})

test_that("excel_format(guess = TRUE) == format_from_signature()", {
  orig_files <- c(
    test_sheet("iris-excel-xlsx.xlsx"),
    test_sheet("iris-excel-xls.xls"),
    file.path(R.home("doc"), "html", "logo.jpg")
  )
  no_ext_copies <- paste0(
    basename(tools::file_path_sans_ext(orig_files)),
    "-no-ending"
  )
  no_ext_copies <- file.path(tempdir(), no_ext_copies)
  file.copy(orig_files, no_ext_copies)
  on.exit(file.remove(no_ext_copies))
  expect_identical(
    ## include one non-existent file, but do not try to remove it
    excel_format(c(no_ext_copies, "i_do_not_exist")),
    c("xlsx", "xls", NA, NA)
  )
})
