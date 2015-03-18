#' Read xls and xlsx files.
#'
#' @param path Path to the xls/xlsx file
#' @param sheet Sheet to read. Either a string (the name of a sheet), or
#'   a integer (the position of the sheet). Defaults to the first sheet.
#' @param col_names Either \code{TRUE} to use the first row as column names,
#'   \code{FALSE} to number columns sequentially from \code{X1} to \code{Xn},
#'   or a character vector giving a name for each column.
#' @param col_types Either \code{NULL} to guess from the spreadsheet or a
#'   character containing "blank", "numeric", "date" or "text".
#' @param na Missing value. By default exell converts blank cells to missing
#'   data. Set this value if you have used a sentinel value for missing values.
#' @param skip Number of rows to skip before reading any data.
#' @export
read_excel <- function(path, sheet = 1, col_names = TRUE, col_types = NULL,
                       na = "", skip = 0) {

  path <- check_file(path)
  ext <- tools::file_ext(path)

  switch(ext,
    xls = read_xls(path, sheet, col_names, col_types, na, skip),
    xlsx = read_xlsx(path, sheet, col_names, col_types, na, skip),
    stop("Don't know how to parse extension ", ext, call. = FALSE)
  )
}

read_xls <- function(path, sheet = 1, col_names = TRUE, col_types = NULL,
                     na = "", skip = 0) {

  sheet <- standardise_sheet(sheet, xls_sheets(path))

  if (isTRUE(col_names)) {
    col_names <- xls_col_names(path, sheet, nskip = skip)
    skip <- skip + 1
  } else if (isFALSE(col_names)) {
    col_names <- paste0("X", seq_along(xls_col_names(path, sheet)))
  }

  if (is.null(col_types)) {
    col_types <- xls_col_types(path, sheet, na = na, nskip = skip)
  }

  xls_cols(path, sheet, col_names = col_names, col_types = col_types, na = na,
    nskip = skip)
}

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

# Helper functions -------------------------------------------------------------

standardise_sheet <- function(sheet, sheet_names) {
  if (length(sheet) != 1) {
    stop("`sheet` must have length 1", call. = FALSE)
  }

  if (is.numeric(sheet)) {
    floor(sheet) - 1L
  } else if (is.character(sheet)) {
    if (!(sheet %in% sheet_names)) {
      stop("Sheet '", sheet, "' not found", call. = FALSE)
    }
    match(sheet, sheet_names) - 1L
  } else {
    stop("`sheet` must be either an integer or a string.", call. = FALSE)
  }
}
