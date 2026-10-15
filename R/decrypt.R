# An encrypted xlsx is an OLE2 compound document and has the same D0 CF 11 E0
# signature as a legacy xls file. The compound document wraps the real xlsx zip
# inside `EncryptionInfo` and `EncryptedPackage` streams. `ole_kind_()` (in
# C++) classifies such files by their streams.
is_encrypted_xlsx <- function(path) {
  identical(ole_kind_(path), "encrypted_xlsx")
}

# Decrypts an encrypted xlsx to a tempfile and returns its path. The caller is
# responsible for checking `is_encrypted_xlsx()` first and for deleting the
# tempfile.
decrypt_xlsx <- function(path, password, call = rlang::caller_env()) {
  password <- obtain_password(password, call = call)
  password <- iconv(enc2utf8(password), "UTF-8", "UTF-16LE", toRaw = TRUE)
  decrypted <- tryCatch(
    xlsx_decrypt_(path, password[[1]]),
    error = function(cnd) {
      cli::cli_abort(
        "Failed to decrypt {.path {path}}.",
        call = call,
        parent = cnd
      )
    }
  )
  if (is.null(decrypted)) {
    cli::cli_abort(
      c(
        "{.arg path} appears to be a password-encrypted xlsx, but decryption failed.",
        "i" = "Perhaps the password is incorrect?"
      ),
      call = call
    )
  }

  out <- tempfile(fileext = ".xlsx")
  writeBin(decrypted, out)
  out
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
