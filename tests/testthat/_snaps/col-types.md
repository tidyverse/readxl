# illegal col_types are rejected

    Code
      read_excel(test_sheet("types.xlsx"), col_types = c("foo", "numeric", "text",
        "bar"))
    Error <simpleError>
      Illegal column type: 'foo' [1], 'bar' [4]

# request for 'blank' col type gets deprecation message and fix

    Code
      read_excel(test_sheet("types.xlsx"), col_types = rep_len(c("blank", "text"),
      length.out = 5))
    Message <simpleMessage>
      `col_type = "blank"` deprecated. Use "skip" instead.
    Output
      # A tibble: 5 x 2
        boolean numeric
        <chr>   <chr>  
      1 TRUE    1      
      2 FALSE   2      
      3 <NA>    <NA>   
      4 TRUE    3      
      5 FALSE   6      

# invalid col_types are rejected

    Code
      read_excel(test_sheet("types.xlsx"), col_types = character())
    Error <simpleError>
      length(col_types) > 0 is not TRUE

---

    Code
      read_excel(test_sheet("types.xlsx"), col_types = 1:3)
    Error <simpleError>
      is.character(col_types) is not TRUE

---

    Code
      read_excel(test_sheet("types.xlsx"), col_types = c(NA, "text", "numeric"))
    Error <simpleError>
      !anyNA(col_types) is not TRUE

# guess_max is honored for col_types

    Code
      types <- read_excel(test_sheet("types.xlsx"), sheet = "guess_max", guess_max = 2)
    Warning <simpleWarning>
      Expecting numeric in A4 / R4C1: got 'ab'

---

    Code
      types <- read_excel(test_sheet("types.xls"), sheet = "guess_max", guess_max = 2)
    Warning <simpleWarning>
      Expecting numeric in A4 / R4C1: got 'ab'

# wrong length col types generates error

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), col_types = c("numeric", "text"))
    Error <simpleError>
      Sheet 1 has 5 columns, but `col_types` has length 2.

---

    Code
      read_excel(test_sheet("iris-excel-xls.xls"), col_types = c("numeric", "text"))
    Error <simpleError>
      Sheet 1 has 5 columns, but `col_types` has length 2.

# numeric is correctly coerced to logical [xlsx]

    Code
      df <- read_xlsx(test_sheet("missing-values-xlsx.xlsx"), guess_max = 0)
    Warning <simpleWarning>
      Expecting logical in A2 / R2C1: got 'NA'
      Expecting logical in B2 / R2C2: got 'NA'

# numeric is correctly coerced to logical [xls]

    Code
      df <- read_xls(test_sheet("missing-values-xls.xls"), guess_max = 0)
    Warning <simpleWarning>
      Expecting logical in A2 / R2C1: got 'NA'
      Expecting logical in B2 / R2C2: got 'NA'

