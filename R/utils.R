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


format_from_extension <- function(path) {
  ext <- tolower(tools::file_ext(path))

  switch(ext,
         xls = "xls",
         xlsx = "xlsx",
         xlsm = "xlsx",
         stop("Unknown format .", ext, call. = FALSE)
  )
}

is_absolute_path <- function(path) {
  grepl("^(/|[A-Za-z]:|\\\\|~)", path)
}

isFALSE <- function(x) identical(x, FALSE)
