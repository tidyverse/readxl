# Decryption of password-protected xlsx files (#794) --------------------------

# An encrypted xlsx is an OLE2 compound document (same D0 CF 11 E0 signature as
# a legacy xls file) that wraps the real xlsx zip inside `EncryptionInfo` and
# `EncryptedPackage` streams. `resolve_encryption()` probes for this, decrypts
# to a tempfile when a `password` is supplied, and surfaces clean, classed
# errors otherwise. The caller is responsible for unlinking a decrypted
# tempfile (see the `decrypted` element).
resolve_encryption <- function(path, password, call = rlang::caller_env()) {
  encrypted <- xlsx_is_encrypted_(path)

  if (!encrypted) {
    if (!is.null(password)) {
      cli::cli_abort(
        c(
          "{.arg password} was supplied, but {.arg path} is not an encrypted \\
          file.",
          i = "readxl can only decrypt ECMA-376 (agile or standard) encrypted \\
          xlsx files."
        ),
        class = "readxl_error_password_unsupported",
        call = call
      )
    }
    return(list(path = path, format = NULL, decrypted = FALSE))
  }

  if (is.null(password)) {
    cli::cli_abort(
      c(
        "{.arg path} is a password-encrypted file.",
        i = "Supply the {.arg password} to read it."
      ),
      class = "readxl_error_password_required",
      call = call
    )
  }
  check_password(password, call = call)

  out <- tempfile(fileext = ".xlsx")
  ok <- xlsx_decrypt_(path, password, out)
  if (!isTRUE(ok)) {
    unlink(out)
    cli::cli_abort(
      "The {.arg password} is incorrect.",
      class = "readxl_error_bad_password",
      call = call
    )
  }

  list(path = out, format = "xlsx", decrypted = TRUE)
}

check_password <- function(password, call = rlang::caller_env()) {
  if (!is_string(password) || is.na(password)) {
    cli::cli_abort(
      "{.arg password} must be a single string.",
      class = "readxl_error_bad_password_arg",
      call = call
    )
  }
  invisible(password)
}
