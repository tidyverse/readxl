#' List all sheets in an excel spreadsheet
#'
#' The list of sheet names is especially useful when you want to iterate over
#' all of the sheets in a workbook. The [readxl Workflows article](https://readxl.tidyverse.org/articles/readxl-workflows.html)
#' provides several worked examples of this, showing how to combine readxl with
#' other packages in the tidyverse, such as purrr, or with base R functions like
#' [lapply()].
#'
#' @inheritParams read_excel
#' @export
#' @examples
#' excel_sheets(readxl_example("datasets.xlsx"))
#' excel_sheets(readxl_example("datasets.xls"))
#'
#' # To load all sheets in a workbook, use lapply()
#' path <- readxl_example("datasets.xls")
#' lapply(excel_sheets(path), read_excel, path = path)
excel_sheets <- function(path, password = NULL) {
  path <- check_file(path)
  format <- check_format(path)
  # An encrypted xlsx file might present as "xls" at this point, because it
  # shares the same D0 CF 11 E0 signature as a legacy xls file.
  # But it's inefficient to test every "xls" for possibly being encrypted xlsx.
  # First, try the obvious thing!
  sheets <- tryCatch(
    switch(format, xls = xls_sheets(path), xlsx = xlsx_sheets(path)),
    error = identity
  )

  if (!inherits(sheets, "error")) {
    return(sheets)
  }

  # The obvious stuff failed, so now we entertain the possibility that this is
  # an encrypted xlsx.
  original_error <- sheets
  enc <- resolve_encryption(path, password)
  if (!enc$decrypted) {
    stop(original_error)
  }
  on.exit(unlink(enc$path), add = TRUE)

  xlsx_sheets(enc$path)
}
