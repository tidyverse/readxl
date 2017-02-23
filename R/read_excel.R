#' @useDynLib readxl
#' @importFrom Rcpp sourceCpp
NULL

#' Read xls and xlsx files.
#'
#' @param path Path to the xls/xlsx file
#' @param sheet Sheet to read. Either a string (the name of a sheet), or an
#'   integer (the position of the sheet). Defaults to the first sheet.
#' @param col_names `TRUE` to use the first row as column names, `FALSE` to get
#'   default names, or a character vector giving a name for each column. If user
#'   provides `col_types` as a vector, `col_names` can have one entry per
#'   column, i.e. have the same length as `col_types`, or one entry per
#'   unskipped column.
#' @param col_types Either `NULL` to guess from the spreadsheet or a character
#'   vector containing one entry per column from these options: "skip",
#'   "numeric", "date", "text" or "list". The content of a cell in a skipped column is
#'   never read and that column will not appear in the data frame output. A list cell
#'   loads a column as a list of length 1 vectors, which are typed using the type
#'   guessing logic from `col_types = NULL`, but on a cell-by-cell basis.
#' @param na Character vector of strings to use for missing values. By default,
#'   readxl treats blank cells as missing data.
#' @param skip Number of rows to skip before reading any data. Leading blank
#'   rows are automatically skipped.
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
#'
#' # "list" col_type can handle information of disparate types
#' df <- read_excel(readxl_example("clippy.xlsx"), col_types = c("text", "list"))
#' df
#' df$value
read_excel <- function(path, sheet = 1L, col_names = TRUE, col_types = NULL,
                       na = "", skip = 0, guess_max = 1000) {
  read_excel_(
    path = path, sheet = sheet,
    col_names = col_names, col_types = col_types,
    na = na, skip = skip, guess_max = guess_max,
    excel_format(path)
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
  read_excel_(
    path = path, sheet = sheet,
    col_names = col_names, col_types = col_types,
    na = na, skip = skip, guess_max = guess_max,
    format = "xls"
  )
}

#' @rdname read_excel
#' @export
read_xlsx <- function(path, sheet = 1L, col_names = TRUE, col_types = NULL,
                      na = "", skip = 0, guess_max = 1000) {
  read_excel_(
    path = path, sheet = sheet,
    col_names = col_names, col_types = col_types,
    na = na, skip = skip, guess_max = guess_max,
    format = "xlsx"
  )
}

read_excel_ <- function(path, sheet = 1L, col_names = TRUE, col_types = NULL,
                        na = "", skip = 0, guess_max = 1000, format) {
  if (format == "xls") {
    sheets_fun <- xls_sheets
    read_fun <- read_xls_
  } else {
    sheets_fun <- xlsx_sheets
    read_fun <- read_xlsx_
  }
  sheet <- standardise_sheet(sheet, sheets_fun(path))
  guess_max <- check_guess_max(guess_max)
  col_types <- check_col_types(col_types)
  tibble::repair_names(
    tibble::as_tibble(
      read_fun(path = path, sheet = sheet,
               col_names = col_names, col_types = col_types,
               na = na, nskip = skip, guess_max = guess_max),
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

check_col_types <- function(col_types) {
  if (is.null(col_types)) {
    return(col_types)
  }
  stopifnot(is.character(col_types), length(col_types) > 0, !anyNA(col_types))

  blank <- col_types == "blank"
  if (any(blank)) {
    message("`col_type = \"blank\"` deprecated. Use \"skip\" instead.")
    col_types[blank] <- "skip"
  }

  accepted_types <- c("skip", "numeric", "date", "text", "list")
  ok <- col_types %in% accepted_types
  if (any(!ok)) {
    info <- paste(
      paste0("'", col_types[!ok], "' [", seq_along(col_types)[!ok], "]"),
      collapse = ", "
    )
    stop(paste("Illegal column type:", info), call. = FALSE)
  }
  col_types
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
