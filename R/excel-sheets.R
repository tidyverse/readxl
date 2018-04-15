#' List all sheets in an excel spreadsheet
#'
#' @inheritParams read_excel
#' @export
#' @examples
#' excel_sheets(readxl_example("datasets.xlsx"))
#' excel_sheets(readxl_example("datasets.xls"))
#'
#' # To load all sheets in a workbook, use lapply
#' path <- readxl_example("datasets.xls")
#' lapply(excel_sheets(path), read_excel, path = path)
excel_sheets <- function(path) {
  format <- check_format(path)

  switch(format,
    xls = xls_sheets(path),
    xlsx = xlsx_sheets(path)
  )
}
