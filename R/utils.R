check_file <- function(path) {
  if (!is_string(path)) {
    stop("`path` must be a string", call. = FALSE)
  }

  if (!file.exists(path)) {
    stop("`path` does not exist: ", sQuote(path), call. = FALSE)
  }
  # We already know the file exists, so we can use `mustWork = FALSE`.
  # Why would we want to do that?
  # https://github.com/tidyverse/readxl/issues/730
  # There are reports of a spurious "Access is denied" warning when the
  # file lives on a Windows network share whose parent directory is not
  # accessible to the user, even though the file itself is readable.
  normalizePath(path, mustWork = FALSE)
}

is_integerish <- function(x) {
  floor(x) == x
}

is_string <- function(x) {
  length(x) == 1 && is.character(x)
}
