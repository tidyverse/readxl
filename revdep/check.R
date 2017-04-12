library("devtools")

revdep_check()
revdep_check_save_summary()
revdep_check_print_problems()

## R --vanilla -e '.libPaths("~/resources/R/revdep_library");options(devtools.revdep.libpath = "~/resources/R/revdep_library")' -e 'source("revdep/check.R")'
