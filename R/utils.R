check_file <- function(path) {
  if (!is_string(path)) {
    stop("`path` must be a string", call. = FALSE)
  }

  if (!file.exists(path)) {
    stop("`path` does not exist: ", sQuote(path), call. = FALSE)
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

is_string <- function(x) {
  length(x) == 1 && is.character(x)
}
