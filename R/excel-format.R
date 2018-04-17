#' Determine file format
#'
#' @description Determine if files are xls or xlsx (or from the xlsx family).
#'
#' @description `excel_format(guess = TRUE)` is used by `read_excel()` to
#'   determine format. It draws on logic from two lower level functions:
#'   * `format_from_ext()` attempts to determine format from the file extension.
#'   * `format_from_signature()` consults the [file
#'   signature](https://en.wikipedia.org/wiki/List_of_file_signatures) or "magic
#'   number".
#'
#' @description File extensions associated with xlsx vs. xls:
#'   * xlsx: `.xlsx`, `.xlsm`, `.xltx`, `.xltm`
#'   * xls: `.xls`
#'
#' @description File signatures (in hexadecimal) for xlsx vs xls:
#'   * xlsx: First 4 bytes are `50 4B 03 04`
#'   * xls: First 8 bytes are `D0 CF 11 E0 A1 B1 1A E1`
#'
#' @inheritParams read_excel
#' @param guess Logical. If the file extension is absent or not recognized, this
#'   controls whether we attempt to guess format based on the file signature or
#'   "magic number".
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
  format <- format_from_ext(path)
  if (!isTRUE(guess)) {
    return(format)
  }
  guess_me <- is.na(format) & file.exists(path)
  format[guess_me] <- format_from_signature(path[guess_me])
  format
}

#' @rdname excel_format
#' @export
format_from_ext <- function(path) {
  ext <- tolower(tools::file_ext(path))

  formats <- c(
    xls = "xls",
    xlsx = "xlsx",
    xlsm = "xlsx",
    xltx = "xlsx",
    xltm = "xlsx"
  )
  unname(formats[ext])
}

#' @rdname excel_format
#' @export
format_from_signature <- function(path) {
  signature <- lapply(path, first_8_bytes)
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

check_format <- function(path) {
  format <- excel_format(path)
  if (is.na(format)) {
    stop("Can't establish that the input is either xls or xlsx.", call. = FALSE)
  }
  format
}
