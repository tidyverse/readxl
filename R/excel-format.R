#' Determine file format
#'
#' Determine if files are xlsx or xls. First the file extension is consulted. If
#' that is unsuccessful and `guess = TRUE` and the file exists, the format is
#' guessed from the [file
#' signature](https://en.wikipedia.org/wiki/List_of_file_signatures) or "magic
#' number".
#'
#' @inheritParams read_excel
#' @param guess Logical. Whether to guess format based on the file itself, if
#'   the extension is neither `"xlsx"` nor `"xls"`.
#'
#' @return Character vector with values `"xlsx"`, `"xls"`, or `NA`.
#' @export
#'
#' @examples
#' files <- c(
#'   "a.xlsx",
#'   "b.xls",
#'   "c.png",
#'   file.path(R.home("doc"), "html", "logo.jpg"),
#'   readxl_example("clippy.xlsx"),
#'   readxl_example("deaths.xls")
#' )
#' excel_format(files)
excel_format <- function(path, guess = TRUE) {
  ext <- tolower(tools::file_ext(path))

  formats <- c(xls = "xls", xlsx = "xlsx", xlsm = "xlsx")
  format <- unname(formats[ext])

  if (!guess || !anyNA(format)) {
    return(format)
  }

  guess_me <- is.na(format) & file.exists(path)
  format[guess_me] <- guess_format(path[guess_me])
  format
}

guess_format <- function(x) {
  signature <- lapply(x, first_8_bytes)
  vapply(signature, sig_to_fmt, "xlsx?")
}

first_8_bytes <- function(x) readBin(x, n = 8, what = "raw")

sig_to_fmt <- function(x) {
  ## https://en.wikipedia.org/wiki/List_of_file_signatures
  xlsx_sig <- as.raw(c(
    "0x50", "0x4B", "0x03", "0x04"
  ))
  xls_sig <- as.raw(c(
    "0xD0", "0xCF", "0x11", "0xE0", "0xA1", "0xB1", "0x1A", "0xE1"
  ))

  if (identical(x[1:4], xlsx_sig)) {
    "xlsx"
  } else if (identical(x, xls_sig)) {
    "xls"
  } else {
    NA_character_
  }
}
