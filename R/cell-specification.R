#' Specify cells for reading
#'
#' If you aren't targetting all the cells in a worksheet, you can request that
#' readxl limit a read to a specific rectangle of cells. The simplest usage is
#' to specify an Excel-like cell range, such as `range = "D12:F15"` or `range =
#' "R1C12:R6C15"`. The cell rectangle can be specified in various other ways,
#' using helper functions. In all cases, cell range processing is handled by the
#' [cellranger][cellranger] package, where you can find full
#' documentation for the functions used in the examples below.
#'
#' @examples
#' \dontrun{
#' gs_gap() %>% gs_read(ws = 2, range = "A1:D8")
#' gs_gap() %>% gs_read(ws = "Europe", range = cell_rows(1:4))
#' gs_gap() %>% gs_read(ws = "Europe", range = cell_rows(100:103),
#'                      col_names = FALSE)
#' gs_gap() %>% gs_read(ws = "Africa", range = cell_cols(1:4))
#' gs_gap() %>% gs_read(ws = "Asia", range = cell_limits(c(1, 5), c(4, NA)))
#' }
#'
#' @seealso The [cellranger][cellranger] package has full documentation on cell
#'   specification and offers additional functions for manipulating "A1:D10"
#'   style spreadsheet ranges. Here are the most relevant:
#'   \itemize{
#'     \item [cellranger::cell_limits()]
#'     \item [cellranger::cell_rows()]
#'     \item [cellranger::cell_cols()]
#'     \item [cellranger::anchored()]
#'   }
#' @name cell-specification
NULL

#' @importFrom cellranger cell_limits
#' @name cell_limits
#' @export
#' @rdname cell-specification
NULL

#' @importFrom cellranger cell_rows
#' @name cell_rows
#' @export
#' @rdname cell-specification
NULL

#' @importFrom cellranger cell_cols
#' @name cell_cols
#' @export
#' @rdname cell-specification
NULL

#' @importFrom cellranger anchored
#' @name anchored
#' @export
#' @rdname cell-specification
NULL
