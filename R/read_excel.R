#' @useDynLib readxl
#' @importFrom Rcpp sourceCpp
NULL

#' Read xls and xlsx files.
#'
#' @param path Path to the xls/xlsx file
#' @param sheet Sheet to read. Either a string (the name of a sheet), or an
#'   integer (the position of the sheet). Defaults to the first sheet.
#' @param col_names `TRUE` to use the first row as column names, `FALSE`
#'   to get default names, or a character vector giving a name for each column.
#' @param col_types Either `NULL` to guess from the spreadsheet or a character
#'   vector containing one entry per column from these options: "blank",
#'   "numeric", "date" or "text".
#' @param na Character vector of strings to use for missing values. By default
#'   readxl treats blank cells as missing data.
#' @param skip Number of rows to skip before reading any data.
#' @param guess_max Maximum number of rows to use for guessing column types.
#' @export
#' @examples
#' datasets <- readxl_example("datasets.xlsx")
#' read_excel(datasets)
#'
#' # Specific sheet either by position or by name
#' read_excel(datasets, 2)
#' read_excel(datasets, "mtcars")
#'
#' # Skipping rows and using default column names
#' read_excel(datasets, skip = 148, col_names = FALSE)
read_excel <- function(path, sheet = 1, col_names = TRUE, col_types = NULL,
                       na = "", skip = 0, guess_max = 1000) {

  path <- check_file(path)
  guess_max <- check_guess_max(guess_max)

  switch(excel_format(path),
    xls =  read_xls(path, sheet, col_names, col_types, na, skip, guess_max),
    xlsx = read_xlsx(path, sheet, col_names, col_types, na, skip, guess_max)
  )
}

#' While `read_excel()` auto detects the format from the file
#' extension, `read_xls()` and `read_xlsx()` can be used to
#' read files without extension.
#'
#' @rdname read_excel
#' @export
read_xls <- function(path, sheet = 1L, col_names = TRUE, col_types = NULL,
                     na = "", skip = 0, guess_max = 1000) {

  sheet <- standardise_sheet(sheet, xls_sheets(path))

  has_col_names <- isTRUE(col_names)
  if (has_col_names) {
    col_names <- xls_col_names(path, sheet, nskip = skip)
  } else if (isFALSE(col_names)) {
    col_names <- rep.int("", length(xls_col_names(path, sheet)))
  }

  if (is.null(col_types)) {
    col_types <- xls_col_types(path, sheet, na = na, nskip = skip,
                               has_col_names = has_col_names, n = guess_max)
  }

  tibble::repair_names(
    tibble::as_tibble(
      xls_cols(path, sheet, col_names = col_names, col_types = col_types,
               na = na, nskip = skip + has_col_names),
      validate = FALSE
    ),
    prefix = "X", sep = "__"
  )
}

#' @rdname read_excel
#' @export
read_xlsx <- function(path, sheet = 1L, col_names = TRUE, col_types = NULL,
                      na = "", skip = 0, guess_max = 1000) {
  path <- check_file(path)
  sheet <- standardise_sheet(sheet, xlsx_sheets(path))

  tibble::repair_names(
    tibble::as_tibble(
      read_xlsx_(path, sheet, col_names, col_types, na,
                 nskip = skip, n_max = guess_max),
      validate = FALSE
    ),
    prefix = "X", sep = "__"
  )
}

# Helper functions -------------------------------------------------------------

excel_format <- function(path) {
  ext <- tolower(tools::file_ext(path))

  switch(
    ext,
    xls = "xls",
    xlsx = "xlsx",
    xlsm = "xlsx",
    if (nzchar(ext)) {
      stop("Unknown file extension: ", ext, call. = FALSE)
    } else {
      stop("Missing file extension.", call. = FALSE)
    }
  )
}

standardise_sheet <- function(sheet, sheet_names) {
  if (length(sheet) != 1) {
    stop("`sheet` must have length 1", call. = FALSE)
  }

  if (is.numeric(sheet)) {
    if (sheet < 1) {
      stop("`sheet` must be positive", call. = FALSE)
    }
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

## from readr
check_guess_max <- function(guess_max, max_limit = .Machine$integer.max %/% 100) {

  if (length(guess_max) != 1 || !is.numeric(guess_max) || !is_integerish(guess_max) ||
      is.na(guess_max) || guess_max < 0) {
    stop("`guess_max` must be a positive integer", call. = FALSE)
  }

  if (guess_max > max_limit) {
    warning("`guess_max` is a very large value, setting to `", max_limit,
            "` to avoid exhausting memory", call. = FALSE)
    guess_max <- max_limit
  }
  guess_max
}
