# xlsx is not read as xls and vice versa

    Code
      read_xls(test_sheet("iris-excel-xlsx.xlsx"))
    Error <simpleError>
      
        filepath: /Users/jenny/rrr/readxl/tests/testthat/sheets/iris-excel-xlsx.xlsx
        libxls error: Unable to open file

---

    Code
      read_xlsx(test_sheet("iris-excel-xls.xls"))
    Error <simpleError>
      zip file '/Users/jenny/rrr/readxl/tests/testthat/sheets/iris-excel-xls.xls' cannot be opened

# non-existent file throws error

    Code
      read_excel("foo")
    Error <simpleError>
      `path` does not exist: 'foo'

# read_excel catches invalid guess_max

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = NA)
    Error <simpleError>
      `guess_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = -1)
    Error <simpleError>
      `guess_max` must be a positive integer

---

    Code
      out <- read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = Inf)
    Warning <simpleWarning>
      `guess_max` is a very large value, setting to `21474836` to avoid exhausting memory

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = NULL)
    Error <simpleError>
      `guess_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = 1:2)
    Error <simpleError>
      `guess_max` must be a positive integer

# read_excel catches invalid n_max

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = NA)
    Error <simpleError>
      `n_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = -1)
    Error <simpleError>
      `n_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = NULL)
    Error <simpleError>
      `n_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = 1:2)
    Error <simpleError>
      `n_max` must be a positive integer

# sheet must be integer or string

    Code
      read_excel(test_sheet("mtcars.xls"), sheet = TRUE)
    Error <simpleError>
      `sheet` must be either an integer or a string.

# trim_ws must be a logical

    Code
      read_excel(test_sheet("mtcars.xls"), trim_ws = "yes")
    Error <simpleError>
      `trim_ws` must be either TRUE or FALSE

