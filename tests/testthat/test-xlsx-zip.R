test_that("ZIP listing distinguishes unreadable paths from damaged archives", {
  path <- withr::local_tempfile(fileext = ".xlsx")
  expect_error(
    zip_list(path),
    "close it and try again",
    class = "readxl_error_cant_open_file"
  )

  writeBin(as.raw(c(0x50, 0x4b, 0x03, 0x04)), path)
  base_error <- expect_error(
    utils::unzip(path, list = TRUE),
    class = "simpleError"
  )
  zip_error <- expect_error(zip_list(path), class = "simpleError")
  expect_identical(conditionMessage(zip_error), conditionMessage(base_error))
})
