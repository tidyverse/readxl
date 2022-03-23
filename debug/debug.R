devtools::clean_dll()
devtools::load_all()

# Here are two small example files.
# They both contain the same two lines of products and their prices.
# test1.xlsx has the first price formatted as EUR and the second as CHF,
# it will trigger the above warning.
# test2.xlsx has both prices formatted as CHF,
# it will trigger no warning but have both prices read as POSIXct.
#
# dir <- fs::dir_create("investigations/iss633-currency-date-confusion")
#
# fs::file_move("~/Downloads/test1.xlsx", fs::path(dir, "EUR-then-CHF.xlsx"))
# fs::file_move("~/Downloads/test2.xlsx", fs::path(dir, "EUR-only.xlsx"))

read_excel("investigations/iss633-currency-date-confusion/EUR-then-CHF.xlsx")
read_excel("investigations/iss633-currency-date-confusion/EUR-only.xlsx")

"investigations/iss633-currency-date-confusion/EUR-only.xlsx" %>%
  xg_unzip() %>%
  xg_linkify() %>%
  xg_browse()

# EUR-then-CHF
# <numFmts count="2">
#   <numFmt numFmtId="165" formatCode="#,##0.00\ [$CHF]"/>
#   <numFmt numFmtId="168" formatCode="#,##0.00\ [$EUR]"/>
# </numFmts>

# EUR-only
# <numFmts count="1">
#   <numFmt numFmtId="164" formatCode="#,##0.00\ [$CHF]"/>
# </numFmts>
