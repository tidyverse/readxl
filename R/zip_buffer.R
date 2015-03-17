zip_buffer <- function(zip_path, file_path) {
  files <- unzip(zip_path, list = TRUE)

  indx <- match(file_path, files$Name)
  if (is.na(indx)) {
    stop("Couldn't find '", file_path, "' in '", zip_path, "'", call. = FALSE)
  }

  size <- files$Length[indx]

  con <- unz(zip_path, file_path, open = "rb")
  on.exit(close(con), add = TRUE)
  readBin(con, raw(), n = size)
}

xlsx_sheets <- function(path) {
  path <- check_file(path)

  xlsx_sheets_(zip_buffer(path, "xl/workbook.xml"))
}

xlsx_strings <- function(path) {
  path <- check_file(path)

  xlsx_strings_(zip_buffer(path, "xl/sharedStrings.xml"))
}
