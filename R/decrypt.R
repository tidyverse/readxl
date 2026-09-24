# An encrypted xlsx is an OLE2 compound document and has the same D0 CF 11 E0
# signature as a legacy xls file. The compound document wraps the real xlsx zip
# inside `EncryptionInfo` and `EncryptedPackage` streams. `resolve_encryption()`
# probes for this, decrypts to a tempfile when a `password` is supplied, and
# surfaces clean errors otherwise.
#
# The caller is responsible for cleaning up the decrypted tempfile.
resolve_encryption <- function(path, password, call = rlang::caller_env()) {
  encrypted <- is_encrypted_xlsx_(path)

  if (!encrypted) {
    return(list(path = path, decrypted = FALSE))
  }

  password <- obtain_password(password, call = call)

  out <- tempfile(fileext = ".xlsx")
  decrypt_ok <- xlsx_decrypt_(path, password, out)
  if (!decrypt_ok) {
    unlink(out)
    cli::cli_abort(
      c(
        "{.arg path} appears to be a password-encrypted xlsx, but decryption failed.",
        "i" = "Perhaps the password is incorrect?"
      ),
      call = call
    )
  }

  list(path = out, decrypted = TRUE)
}

obtain_password <- function(password, call = rlang::caller_env()) {
  can_prompt <- rlang::is_interactive() && askpass_installed()

  if (is.null(password) && !can_prompt) {
    msg <- "{.arg path} appears to be a password-encrypted xlsx, but no \\
            password is available."
    if (!askpass_installed()) {
      msg <- c(
        msg,
        i = "If the {.pkg askpass} package is installed, readxl can prompt \\
             for the password in interactive sessions."
      )
    }
    msg <- c(
      msg,
      i = "Read the documentation for the {.arg password} argument."
    )
    cli::cli_abort(
      msg,
      call = call
    )
  }

  if (is.null(password)) {
    password <- askpass_askpass
  }

  if (is.function(password)) {
    password <- tryCatch(
      password(),
      error = function(cnd) {
        cli::cli_abort(
          "Failed to obtain a password.",
          call = call,
          parent = cnd
        )
      }
    )
  }

  rlang::check_string(password, arg = "password", call = call)
  invisible(password)
}

# Mockable wrappers around session state and askpass, for tests ---------------

askpass_installed <- function() {
  rlang::is_installed("askpass")
}

askpass_askpass <- function() {
  askpass::askpass()
}
