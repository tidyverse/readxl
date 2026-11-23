# Called only from C++ code, but currently needs to be implemented in R.
zip_buffer <- function(zip_path, file_path) {
  files <- zip_list(zip_path)

  indx <- match(file_path, files$Name)
  if (is.na(indx)) {
    stop("Couldn't find '", file_path, "' in '", zip_path, "'", call. = FALSE)
  }

  size <- files$Length[indx]

  con <- unz(zip_path, file_path, open = "rb")
  on.exit(close(con), add = TRUE)
  readBin(con, raw(), n = size)
}

zip_has_file <- function(zip_path, file_path) {
  file_path %in% zip_list(zip_path)$Name
}

zip_list <- function(zip_path) {
  tryCatch(
    utils::unzip(zip_path, list = TRUE),
    error = function(cnd) {
      # Keep the ZIP error when the path is readable.
      first_8_bytes(zip_path)
      stop(cnd)
    }
  )
}
