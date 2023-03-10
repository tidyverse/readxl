# wrong length column names are rejected

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), col_names = LETTERS[1:3])
    Error <simpleError>
      Sheet 1 has 5 columns (5 unskipped), but `col_names` has length 3.

---

    Code
      read_excel(test_sheet("iris-excel-xls.xls"), col_names = LETTERS[1:3])
    Error <simpleError>
      Sheet 1 has 5 columns (5 unskipped), but `col_names` has length 3.

