# Revdeps

## Failed to check (2)

|package      |version |error |warning |note |
|:------------|:-------|:-----|:-------|:----|
|colleyRstats |?       |      |        |     |
|fio          |1.0.0   |1     |        |     |

colleyRstats requires R >=4.6.0, which isn't supported yet for our cloud checks. But I got a clean `R CMD check` result locally.

fio requires Rust >= 1.84, but apparently version 1.75.0 is what's available in our cloud checks. Locally I have 1.92.0-nightly and I got a clean `R CMD check` result locally (1 NOTE and 1 WARNING but are not readxl related).
