#' List all sheets in an excel spreadsheet.
#'
#' @inheritParams read_excel
#' @export
#' @examples
#' excel_sheets(system.file("extdata/datasets.xlsx", package = "readxl"))
#' excel_sheets(system.file("extdata/datasets.xls", package = "readxl"))
#'
#' # To load all sheets in a workbook, use lapply
#' path <- system.file("extdata/datasets.xls", package = "readxl")
#' lapply(excel_sheets(path), read_excel, path = path)
excel_sheets <- function(path) {
  path <- check_file(path)
  ext <- tolower(tools::file_ext(path))

  switch(excel_format(path),
    xls =  xls_sheets(path),
    xlsx = xlsx_sheets(path)
  )
}
