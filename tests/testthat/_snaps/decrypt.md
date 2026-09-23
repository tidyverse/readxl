# encrypted xlsx without a password errors

    Code
      read_excel(test_sheet("Encrypted.xlsx"))
    Condition
      Error in `read_excel()`:
      ! `path` is a password-encrypted file.
      i Supply the `password` to read it.

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

