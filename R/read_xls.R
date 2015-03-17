#' Read an xls file (Excel 94-2007)
#'
#' @param path Path to the xls file
#' @param sheet Sheet to read. Either a string (the name of a sheet), or
#'   a integer (the position of the sheet). Defaults to the first sheet.
#' @param col_names Either \code{TRUE} to use the first row as column names,
#'   \code{FALSE} to number columns sequentially from \code{X1} to \code{Xn},
#'   or a character vector giving a name for each column.
#' @param col_types Either \code{NULL} to guess from the spreadsheet or a
#'   character containing "blank", "numeric", "date" or "text".
#' @param skip Number of rows to skip before reading any data.
#' @export
read_xls <- function(path, sheet = 1, col_names = TRUE, col_types = NULL, skip = 0) {
  path <- check_file(path)

  if (is.character(sheet)) {
    i <- match(xls_sheets(path), sheet) - 1
    if (is.na(i)) {
      stop("Sheet '", sheet, "' not found", call. = FALSE)
    }
  } else {
    i <- sheet - 1
  }

  if (isTRUE(col_names)) {
    col_names <- xls_col_names(path, i = i, nskip = skip)
    skip <- skip + 1
  } else if (isFALSE(col_names)) {
    col_names <- paste0("X", seq_along())
  }

  if (is.null(col_types)) {
    col_types <- xls_col_types(path, i, nskip = skip)
  }

  xls_cols(path, i, col_names = col_names, col_types = col_types, nskip = skip)
}
