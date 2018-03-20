check_file <- function(path) {
  if (!file.exists(path)) {
    stop("Path does not exist: ", sQuote(path), call. = FALSE)
  }
  path
}

is_integerish <- function(x) {
  floor(x) == x
}
