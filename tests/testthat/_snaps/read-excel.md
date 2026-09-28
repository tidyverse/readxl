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
      Error in `read_xlsx()`:
      ! This `path` does not appear to be an xlsx file:
        'VOLATILE_FILEPATH'
      i Did you mean to call `read_xls()`?

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

# read_excel catches limits too large for an integer

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), n_max = 1e+10)
    Condition
      Error:
      ! Row limits implied by `skip` and `n_max` are too large to be represented as an integer

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), skip = 1e+10)
    Condition
      Error:
      ! Row limits implied by `skip` and `n_max` are too large to be represented as an integer

# sheet must be integer or string

    Code
      read_excel(test_sheet("mtcars.xls"), sheet = TRUE)
    Condition
      Error:
      ! `sheet` must be either an integer or a string.

# sheet must not be NA

    Code
      read_excel(test_sheet("mtcars.xls"), sheet = NA)
    Condition
      Error:
      ! `sheet` must not be `NA`

# trim_ws must be a logical

    Code
      read_excel(test_sheet("mtcars.xls"), trim_ws = "yes")
    Condition
      Error:
      ! `trim_ws` must be either TRUE or FALSE

