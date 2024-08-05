# we get correct dates prior to March 1, 1900, in 1900 date system [xlsx]

    Code
      df <- read_excel(test_sheet("dates-leap-year-1900-xlsx.xlsx"), col_types = c(
        "date", "text", "logical"))
    Condition
      Warning in `read_fun()`:
      NA inserted for impossible 1900-02-29 datetime

# we get correct dates prior to March 1, 1900, in 1900 date system [xls]

    Code
      df <- read_excel(test_sheet("dates-leap-year-1900-xls.xls"), col_types = c(
        "date", "text", "logical"))
    Condition
      Warning in `read_fun()`:
      NA inserted for impossible 1900-02-29 datetime

