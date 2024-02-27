# coercion warnings report correct address

    Code
      read_excel(test_sheet("geometry.xlsx"), sheet = "warning_B6", col_types = "numeric")
    Condition
      Warning:
      Expecting numeric in B6 / R6C2: got 'wtf'
    Output
      # A tibble: 3 x 1
         var1
        <dbl>
      1  1234
      2  2345
      3    NA

---

    Code
      read_excel(test_sheet("geometry.xlsx"), sheet = "warning_AT6", col_types = "numeric")
    Condition
      Warning:
      Expecting numeric in AT6 / R6C46: got 'wtf'
    Output
      # A tibble: 3 x 1
         var1
        <dbl>
      1  1234
      2  2345
      3    NA

---

    Code
      read_excel(test_sheet("geometry.xlsx"), sheet = "warning_AKE6", col_types = "numeric")
    Condition
      Warning:
      Expecting numeric in AKE6 / R6C967: got 'wtf'
    Output
      # A tibble: 3 x 1
         var3
        <dbl>
      1  1234
      2  2345
      3    NA

---

    Code
      read_excel(test_sheet("geometry.xls"), sheet = "warning_B6", col_types = "numeric")
    Condition
      Warning:
      Expecting numeric in B6 / R6C2: got 'wtf'
    Output
      # A tibble: 3 x 1
         var1
        <dbl>
      1  1234
      2  2345
      3    NA

---

    Code
      read_excel(test_sheet("geometry.xls"), sheet = "warning_AT6", col_types = "numeric")
    Condition
      Warning:
      Expecting numeric in AT6 / R6C46: got 'wtf'
    Output
      # A tibble: 3 x 1
         var1
        <dbl>
      1  1234
      2  2345
      3    NA

