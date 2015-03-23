#' List all sheets in an excel spreadsheet.
#'
#' @inheritParams read_excel
#' @export
excel_sheets <- function(path) {
  path <- check_file(path)
  ext <- tolower(tools::file_ext(path))

  switch(excel_format(path),
    xls =  xls_sheets(path),
    xlsx = xlsx_sheets(path)
  )
}
