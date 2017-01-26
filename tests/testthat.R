library(testthat)
library(readxl)

if (requireNamespace("rprojroot")) {
  test_check("readxl")
} else {
  message("Install suggested package rprojroot in order to run the tests.")
}


