# xlsx is not read as xls and vice versa

    Code
      read_xls(test_sheet("iris-excel-xlsx.xlsx"))
    Condition
      Error:
      ! 
        filepath: 'VOLATILE_FILEPATH'
        libxls error: Unable to open file

---

    Code
      read_xlsx(test_sheet("iris-excel-xls.xls"))
    Condition
      Error in `utils::unzip()`:
      ! zip file 'VOLATILE_FILEPATH' cannot be opened

# non-existent file throws error

    Code
      read_excel("foo")
    Condition
      Error:
      ! `path` does not exist: 'foo'

# read_excel catches invalid guess_max

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = NA)
    Condition
      Error:
      ! `guess_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = -1)
    Condition
      Error:
      ! `guess_max` must be a positive integer

---

    Code
      out <- read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = Inf)
    Condition
      Warning:
      `guess_max` is a very large value, setting to `21474836` to avoid exhausting memory

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = NULL)
    Condition
      Error:
      ! `guess_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), guess_max = 1:2)
    Condition
      Error:
      ! `guess_max` must be a positive integer

# read_excel catches invalid n_max

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = NA)
    Condition
      Error:
      ! `n_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = -1)
    Condition
      Error:
      ! `n_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = NULL)
    Condition
      Error:
      ! `n_max` must be a positive integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = 1:2)
    Condition
      Error:
      ! `n_max` must be a positive integer

# sheet must be integer or string

    Code
      read_excel(test_sheet("mtcars.xls"), sheet = TRUE)
    Condition
      Error:
      ! `sheet` must be either an integer or a string.

# trim_ws must be a logical

    Code
      read_excel(test_sheet("mtcars.xls"), trim_ws = "yes")
    Condition
      Error:
      ! `trim_ws` must be either TRUE or FALSE

