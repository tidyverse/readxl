check_file <- function(path) {
  if (!file.exists(path)) {
    stop("'", path, "' does not exist",
      if (!is_absolute_path(path))
        paste0(" in current working directory ('", getwd(), "')"),
      ".",
      call. = FALSE)
  }

  normalizePath(path, "/", mustWork = FALSE)
}

is_absolute_path <- function(path) {
  grepl("^(/|[A-Za-z]:|\\\\|~)", path)
}

isFALSE <- function(x) identical(x, FALSE)
