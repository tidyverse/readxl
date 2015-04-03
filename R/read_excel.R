#' @useDynLib readxl
#' @importFrom Rcpp sourceCpp
NULL

#' Read xls and xlsx files.
#'
#' @param path Path to the xls/xlsx file
#' @param sheet Sheet to read. Either a string (the name of a sheet), or
#'   an integer (the position of the sheet). Defaults to the first sheet.
#' @param col_names Either \code{TRUE} to use the first row as column names,
#'   \code{FALSE} to number columns sequentially from \code{X1} to \code{Xn},
#'   or a character vector giving a name for each column.
#' @param col_types Either \code{NULL} to guess from the spreadsheet or a
#'   character vector containing "blank", "numeric", "date" or "text".
#' @param na Missing value. By default readxl converts blank cells to missing
#'   data. Set this value if you have used a sentinel value for missing values.
#' @param skip Number of rows to skip before reading any data.
#' @export
#' @examples
#' datasets <- system.file("extdata/datasets.xlsx", package = "readxl")
#' read_excel(datasets)
#'
#' # Specific sheet either by position or by name
#' read_excel(datasets, 2)
#' read_excel(datasets, "mtcars")
read_excel <- function(path, sheet = 1, col_names = TRUE, col_types = NULL,
                       na = "", skip = 0) {

  path <- check_file(path)
  ext <- tolower(tools::file_ext(path))

  switch(excel_format(path),
    xls =  read_xls(path, sheet, col_names, col_types, na, skip),
    xlsx = read_xlsx(path, sheet, col_names, col_types, na, skip)
  )
}

read_xls <- function(path, sheet = 1, col_names = TRUE, col_types = NULL,
                     na = "", skip = 0) {

  sheet <- standardise_sheet(sheet, xls_sheets(path))

  has_col_names <- isTRUE(col_names)
  if (has_col_names) {
    col_names <- xls_col_names(path, sheet, nskip = skip)
  } else if (isFALSE(col_names)) {
    col_names <- paste0("X", seq_along(xls_col_names(path, sheet)))
  }

  if (is.null(col_types)) {
    col_types <- xls_col_types(path, sheet, na = na, nskip = skip, has_col_names = has_col_names)
  }

  xls_cols(path, sheet, col_names = col_names, col_types = col_types, na = na,
    nskip = skip + has_col_names)
}

read_xlsx <- function(path, sheet = 1L, col_names = TRUE, col_types = NULL,
                      na = "", skip = 0) {
  path <- check_file(path)
  sheet <- standardise_sheet(sheet, xlsx_sheets(path))

  read_xlsx_(path, sheet, col_names = col_names, col_types = col_types, na = na,
             nskip = skip)
}

# Helper functions -------------------------------------------------------------

excel_format <- function(path) {
  ext <- tolower(tools::file_ext(path))

  switch(ext,
    xls = "xls",
    xlsx = "xlsx",
    xlsm = "xlsx",
    stop("Unknown format .", ext, call. = FALSE)
  )
}

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
