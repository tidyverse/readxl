# encrypted xlsx without a password errors

    Code
      read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"))
    Condition
      Error in `read_xlsx()`:
      ! `path` appears to be a password-encrypted xlsx, but no password is available.
      i Read the documentation for the `password` argument.

# encrypted xlsx with the wrong password errors

    Code
      read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"), password = "wrong")
    Condition
      Error in `read_xlsx()`:
      ! `path` appears to be a password-encrypted xlsx, but decryption failed.
      i Perhaps the password is incorrect?

# a function-valued password that fails errors informatively

    Code
      read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"), password = function()
        stop("no password available"))
    Condition
      Error in `read_xlsx()`:
      ! Failed to obtain a password.
      Caused by error in `password()`:
      ! no password available

# password must be a single string or a function

    Code
      read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"), password = function()
        c("a", "b"))
    Condition
      Error in `read_xlsx()`:
      ! `password` must be a single string, not a character vector.

---

    Code
      read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"), password = 42)
    Condition
      Error in `read_xlsx()`:
      ! `password` must be a single string, not the number 42.

# encrypted xlsx with no password errors when prompting is impossible

    Code
      read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"))
    Condition
      Error in `read_xlsx()`:
      ! `path` appears to be a password-encrypted xlsx, but no password is available.
      i If the askpass package is installed, readxl can prompt for the password in interactive sessions.
      i Read the documentation for the `password` argument.

# a cancelled auto-prompt errors informatively

    Code
      read_xlsx(test_sheet("mtcars-xlsx-encrypted-with-cars.xlsx"))
    Condition
      Error in `read_xlsx()`:
      ! Failed to obtain a password.
      Caused by error in `password()`:
      ! Password prompt cancelled

