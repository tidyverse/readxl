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

