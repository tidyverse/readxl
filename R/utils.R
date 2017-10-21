check_file <- function(path) {
  if (!file.exists(path)) {
    stop("Path does not exist: ", sQuote(path), call. = FALSE)
  }
  path
}

is_absolute_path <- function(path) {
  grepl("^(/|[A-Za-z]:|\\\\|~)", path)
}

isFALSE <- function(x) identical(x, FALSE)

is_integerish <- function(x) {
  floor(x) == x
}
