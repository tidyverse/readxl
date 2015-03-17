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
