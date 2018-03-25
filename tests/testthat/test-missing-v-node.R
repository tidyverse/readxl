context("missing-v-node")

test_that("sheet with missing v node is read without crash", {
  sheet <- read_excel(test_sheet("missing-v-node.xlsx"))
  succeed(message = "There was no crash reading the test file")
})
