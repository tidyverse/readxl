#' Get path to readxl example
#'
#' readxl comes bundled with some example files in its `inst/extdata`
#' directory. This function make them easy to access.
#'
#' @param path Name of file. If `NULL`, the example files will be listed.
#' @export
#' @examples
#' readxl_example()
#' readxl_example("datasets.xlsx")
readxl_example <- function(path = NULL) {
  if (is.null(path)) {
    dir(system.file("extdata", package = "readxl"))
  } else {
    system.file("extdata", path, package = "readxl", mustWork = TRUE)
  }
}
