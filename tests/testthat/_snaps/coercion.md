# contaminated, explicit logical is read as logical

    Code
      df <- read_excel(test_sheet("types.xls"), sheet = "logical_coercion",
      col_types = c("logical", "text"))
    Warning <simpleWarning>
      Expecting logical in A14 / R14C1: got 'cabbage'
      Expecting logical in A18 / R18C1: got a date

---

    Code
      df <- read_excel(test_sheet("types.xlsx"), sheet = "logical_coercion",
      col_types = c("logical", "text"))
    Warning <simpleWarning>
      Expecting logical in A14 / R14C1: got 'cabbage'
      Expecting logical in A18 / R18C1: got a date

