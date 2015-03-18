#' @export
#' @rdname read_xls
read_xlsx <- function(path, sheet = 1L, col_names = TRUE, col_types = NULL,
                      na = "", skip = 0) {
  path <- check_file(path)
  sheet <- standardise_sheet(sheet, xlsx_sheets(path))

  if (isTRUE(col_names)) {
    col_names <- xlsx_col_names(path, sheet, nskip = skip)
    skip <- skip + 1
  } else if (isFALSE(col_names)) {
    col_names <- paste0("X", seq_along(xlsx_col_names(path, sheet)))
  }

  if (is.null(col_types)) {
    col_types <- xlsx_col_types(path, sheet, na = na, nskip = skip)
  }

  xlsx_cols(path, sheet, col_names = col_names, col_types = col_types, na = na,
            nskip = skip)
}
