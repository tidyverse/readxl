# contaminated, explicit logical is read as logical

    Code
      df <- read_excel(test_sheet("types.xls"), sheet = "logical_coercion",
      col_types = c("logical", "text"))
    Condition
      Warning:
      Expecting logical in A14 / R14C1: got 'cabbage'
      Warning:
      Expecting logical in A18 / R18C1: got a date

---

    Code
      df <- read_excel(test_sheet("types.xlsx"), sheet = "logical_coercion",
      col_types = c("logical", "text"))
    Condition
      Warning:
      Expecting logical in A14 / R14C1: got 'cabbage'
      Warning:
      Expecting logical in A18 / R18C1: got a date

# contaminated, explicit date is read as date

    Code
      df <- read_excel(test_sheet("types.xls"), sheet = "date_coercion", col_types = "date")
    Condition
      Warning:
      Expecting date in A4 / R4C1: got boolean
      Warning:
      Expecting date in A5 / R5C1: got 'cabbage'
      Warning:
      Coercing numeric to date in A6 / R6C1
      Warning:
      Coercing numeric to date in A7 / R7C1

---

    Code
      df <- read_excel(test_sheet("types.xlsx"), sheet = "date_coercion", col_types = "date")
    Condition
      Warning:
      Expecting date in A4 / R4C1: got boolean
      Warning:
      Expecting date in A5 / R5C1: got 'cabbage'
      Warning:
      Coercing numeric to date in A6 / R6C1
      Warning:
      Coercing numeric to date in A7 / R7C1

# contaminated, explicit numeric is read as numeric

    Code
      df <- read_excel(test_sheet("types.xls"), sheet = "numeric_coercion",
      col_types = "numeric")
    Condition
      Warning:
      Coercing text to numeric in A3 / R3C1: '72'
      Warning:
      Coercing boolean to numeric in A5 / R5C1
      Warning:
      Expecting numeric in A6 / R6C1: got a date
      Warning:
      Expecting numeric in A7 / R7C1: got 'cabbage'

---

    Code
      df <- read_excel(test_sheet("types.xlsx"), sheet = "numeric_coercion",
      col_types = "numeric")
    Condition
      Warning:
      Coercing text to numeric in A3 / R3C1: '72'
      Warning:
      Coercing boolean to numeric in A5 / R5C1
      Warning:
      Expecting numeric in A6 / R6C1: got a date
      Warning:
      Expecting numeric in A7 / R7C1: got 'cabbage'

