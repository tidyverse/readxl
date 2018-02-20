#' @importFrom utils download.file
check_file <- function(path) {
  if (file.exists(path)) {
    return(path)
  } else if (grepl('^(ht|f)tps?://', path)) {
    # eschew fileext -- leave excel_format/sig_to_fmt to do the
    #   work of determining the file type,
    #   rather than trying to extract this from the URL
    tmpf = tempfile()
    # want to unlink tmpf after exiting read_excel; apply
    #   Yihui Xie/Kevin Ushey's defer trick found here:
    #   https://yihui.name/en/2017/12/on-exit-parent
    do.call(on.exit, list(unlink(tmpf), add = TRUE),
            envir = parent.frame(2L))
    if (grepl('^(ht|f)tps://', path)) {
      if (!requireNamespace('curl', quietly = TRUE))
        stop("Package 'curl' required to read from a secure URL; ",
             "Please run install.packages('curl') and try again.")
      message('Secure URL detected; downloading to ',
              tmpf, ' with curl to proceed reading')
      curl::curl_download(path, tmpf, mode = "wb", quiet = FALSE)
    } else {
      method = getOption('download.file.method', default = 'auto')
      message('URL detected; downloading to ', tmpf,
              ' with ', method, ' to proceed reading. You can ',
              'configure the download method by setting ',
              "options('download.file.method').")
      download.file(input, tmpFile, method = method, mode = "wb", quiet = FALSE)
    }
    # overwrite path with the now-downloaded file, since
    #   this function may be called more than once in the
    #   process of reading (so, if we don't do this,
    #   the file will be downloaded twice)
    assign('path', tmpf, envir = parent.frame(2L))
    return(tmpf)
  } else {
    stop("Input path is neither a file nor a URL: ",
         sQuote(path), call. = FALSE)
  }
}

is_absolute_path <- function(path) {
  grepl("^(/|[A-Za-z]:|\\\\|~)", path)
}

isFALSE <- function(x) identical(x, FALSE)

is_integerish <- function(x) {
  floor(x) == x
}
