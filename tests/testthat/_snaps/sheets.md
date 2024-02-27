# informative error when requesting non-existent sheet by name

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = "tulip")
    Condition
      Error:
      ! Sheet 'tulip' not found

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), range = "tulip!A1:A1")
    Condition
      Error:
      ! Sheet 'tulip' not found

# informative error when requesting non-existent sheet by position

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = 2)
    Condition
      Error:
      ! Can't retrieve sheet in position 2, only 1 sheet(s) found.

---

    Code
      read_excel(test_sheet("iris-excel-xls.xls"), sheet = 2)
    Condition
      Error:
      ! Can't retrieve sheet in position 2, only 1 sheet(s) found.

# invalid sheet values caught

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = 0)
    Condition
      Error:
      ! `sheet` must be positive

---

    Code
      read_excel(test_sheet("iris-excel-xlsx.xlsx"), sheet = rep(1L, 2))
    Condition
      Error:
      ! `sheet` must have length 1

# double specification of sheet generates message and range wins

    Code
      double <- read_excel(test_sheet("sheet-xml-lookup.xlsx"), sheet = "Asia",
      range = "Oceania!A1:A1")
    Message
      Two values given for `sheet`. Using the `sheet` found in `range`:
      Oceania

