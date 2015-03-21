#' Load a file inside a zip into a raw vector
#'
#' @export
#' @keywords internal
zip_buffer <- function(zip_path, file_path) {
  files <- utils::unzip(zip_path, list = TRUE)

  indx <- match(file_path, files$Name)
  if (is.na(indx)) {
    stop("Couldn't find '", file_path, "' in '", zip_path, "'", call. = FALSE)
  }

  size <- files$Length[indx]

  con <- unz(zip_path, file_path, open = "rb")
  on.exit(close(con), add = TRUE)
  readBin(con, raw(), n = size)
}


zip_list <- function(zip_path) {
  unzip(zip_path, list = TRUE)
}
