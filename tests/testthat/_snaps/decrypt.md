# encrypted xlsx without a password errors

    Code
      read_excel(test_sheet("Encrypted.xlsx"))
    Condition
      Error in `read_excel()`:
      ! `path` is a password-encrypted file.
      i Supply the password via the `password` argument, e.g. `password = askpass::askpass` to enter it interactively.

# encrypted xlsx with the wrong password errors

    Code
      read_excel(test_sheet("Encrypted.xlsx"), password = "wrong")
    Condition
      Error in `read_excel()`:
      ! The `password` is incorrect.

# password on a non-encrypted file errors

    Code
      read_excel(test_sheet("Untitled1.xlsx"), password = "msoc")
    Condition
      Error in `read_excel()`:
      ! `password` was supplied, but `path` is not an encrypted file.
      i readxl can only decrypt ECMA-376 (agile or standard) encrypted xlsx files.

---

    Code
      read_xls(test_sheet("iris-excel-xls.xls"), password = "msoc")
    Condition
      Error in `read_xls()`:
      ! Reading password-encrypted xls files is not supported.
      i Only ECMA-376 encrypted xlsx files can be decrypted.

# a function-valued password that fails errors informatively

    Code
      read_excel(test_sheet("Encrypted.xlsx"), password = function() stop(
        "no console"))
    Condition
      Error in `read_excel()`:
      ! The `password` function failed with an error.
      i Interactive prompts, e.g. `askpass::askpass()`, require an interactive session. Otherwise supply the password as a string, possibly retrieved from an environment variable.
      Caused by error in `fun()`:
      ! no console

# password must be a single string or a function

    Code
      read_excel(test_sheet("Encrypted.xlsx"), password = function() c("a", "b"))
    Condition
      Error in `read_excel()`:
      ! `password` must be a single string or a function that returns one.
      i Use `password = askpass::askpass` to enter the password interactively.

---

    Code
      read_excel(test_sheet("Encrypted.xlsx"), password = 42)
    Condition
      Error in `read_excel()`:
      ! `password` must be a single string or a function that returns one.
      i Use `password = askpass::askpass` to enter the password interactively.

