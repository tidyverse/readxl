xlsx <- function(path) {
  files <- utils::unzip(path, list = TRUE)
  names(files) <- tolower(names(files))
  files$date <- NULL
  files <- files[order(files$name), ]
  rownames(files) <- NULL

  structure(list(path = path, files = files), class = "xlsx")
}

#' @export
print.xlsx <- function(x, ...) {
  cat("<xlsx>\n")
  print(x$files)
}

#' @export
`[[.xlsx` <- function(x, i, ...) {
  if (is.numeric(i)) {
    path <- x$files$name[[i]]
  } else if (is.character(i)) {
    path <- i
  } else {
    stop("i not supported", call. = FALSE)
  }

  ext <- tolower(tools::file_ext(path))

  switch(ext,
    xml = zip_xml(x$path, path),
    rels = zip_xml(x$path, path),
    zip_buffer(x$path, path)
  )
  invisible()
}

# ------------------------------------------------------------------------------

# Called only from C++ code, but currently needs to be implemented in R.
zip_buffer <- function(zip_path, file_path) {
  files <- utils::unzip(zip_path, list = TRUE)

  indx <- match(file_path, files$Name)
  if (is.na(indx)) {
    stop("Couldn't find '", file_path, "' in '", zip_path, "'", call. = FALSE)
  }

  size <- files$Length[indx]

  con <- unz(zip_path, file_path, open = "rb")
  on.exit(close(con), add = TRUE)
  readBin(con, raw(), n = size)
}

zip_has_file <- function(zip_path, file_path) {
  file_path %in% utils::unzip(zip_path, list = TRUE)$Name
}
