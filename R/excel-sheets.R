#' List sheets in an excel spreadsheet
#'
#' @inheritParams read_excel
#' @param glob A wildcard to filter sheets.
#' @param regexp A regular expression to filter sheets.
#' @export
#' @examples
#' excel_sheets(readxl_example("datasets.xlsx"))
#' excel_sheets(readxl_example("datasets.xls"))
#'
#' # Filter sheets using certain criteria
#' excel_sheets(readxl_example("datasets.xls"), glob = "??i*")
#' excel_sheets(readxl_example("datasets.xls"), regexp = "(r|e)s$")
#'
#' # To load all sheets in a workbook, use lapply
#' path <- readxl_example("datasets.xls")
#' lapply(excel_sheets(path), read_excel, path = path)
excel_sheets <- function(path, glob = NULL, regexp = NULL) {
  path <- check_file(path)
  format <- check_format(path)

  sheets <- switch(format,
    xls = xls_sheets(path),
    xlsx = xlsx_sheets(path)
  )

  if (!is.null(glob)) {
    if (!is_string(glob)) {
      stop("`glob` must be a string.", call. = FALSE)
    }
    if (!is.null(regexp)) {
      stop("use either `glob` or `regexp`, not both.", call. = FALSE)
    }
    regexp <- utils::glob2rx(glob)
  }

  if (!is.null(regexp)) {
    if (!is_string(regexp)) {
      stop("`regexp` must be a string.", call. = FALSE)
    }
    sheets <- grep(x = sheets, pattern = regexp, value = TRUE)
  }

  if (length(sheets) == 0) {
    stop("can not find sheets with the specified criteria.", call. = FALSE)
  }

  return(sheets)
}
