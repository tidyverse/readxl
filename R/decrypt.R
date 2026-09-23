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
    # A string password for an unencrypted file probably indicates user
    # confusion. A function means "a password, if needed" and is never called.
    if (!is.null(password) && !is.function(password)) {
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
        i = "Supply the password via the {.arg password} argument, e.g. \\
        {.code password = askpass::askpass} to enter it interactively."
      ),
      class = "readxl_error_password_required",
      call = call
    )
  }
  password <- check_password(password, call = call)

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
  if (is.function(password)) {
    password <- call_password_function(password, call = call)
  }
  if (!is_string(password) || is.na(password)) {
    cli::cli_abort(
      c(
        "{.arg password} must be a single string or a function that returns \\
        one.",
        i = "Use {.code password = askpass::askpass} to enter the password \\
        interactively."
      ),
      class = "readxl_error_bad_password_arg",
      call = call
    )
  }
  invisible(password)
}

call_password_function <- function(fun, call = rlang::caller_env()) {
  tryCatch(
    fun(),
    error = function(cnd) {
      cli::cli_abort(
        c(
          "The {.arg password} function failed with an error.",
          i = "Interactive prompts, e.g. {.fun askpass::askpass}, require an \\
          interactive session. Otherwise supply the password as a string, \\
          possibly retrieved from an environment variable."
        ),
        class = "readxl_error_password_function",
        call = call,
        parent = cnd
      )
    }
  )
}
