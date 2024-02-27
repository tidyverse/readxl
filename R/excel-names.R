#' List all defined names in an excel spreadsheet, with any associated range reference
#'
#' The list of defined names is especially useful when you want to avoid hard-coding
#' the range of cells to be read from a workbook. Instead, if a range of cells to be
#' read is named in the workbook, this function allows you to obtain the actual cell
#' range, which may then be used in a subsequent call to [excel_names] to read the
#' range.
#'
#' @inheritParams excel_names
#' @export
#' @examples
#' names <- excel_names(readxl_example("names.xlsx"))
#' ref <- names[names$name == "data"]$ref
#' read_excel(readxl_example("names.xlsx"), range = ref)
excel_names <- function(path) {
  path <- check_file(path)
  format <- check_format(path)
  path <- normalizePath(path)

  result <- switch(format,
    xls = xls_names(path),
    xlsx = xlsx_names(path)
  )

  tibble::as_tibble(result)
}
