library("devtools")

## manual installations into ~/resources/R/revdep_library
##  * devtools
##  * rJava
##  * cairoDevice
##  * RGtk2
##  * gWidgetsRGtk2
##  * RGtk2Extras
##  * openNLP
##  * openNLPdata
##  * ncdf4
##  * qdap
##  * pdftools
##  * proj4
##  * rjags
##  * playwith
##  * RODBC nope nope did not install :( come back to
##  * rggobi nope nope did not install :( come back to this

ignore <- c(
  "dynBiplotGUI", ## hangs: * checking whether package ‘dynBiplotGUI’ can be installed ...
  "ie2misc" ## java something something java something
)
revdep_check(ignore = ignore)
revdep_check_save_summary()
revdep_check_print_problems()

## R --vanilla -e '.libPaths("~/resources/R/revdep_library");options(devtools.revdep.libpath = "~/resources/R/revdep_library")' -e 'source("revdep/check.R")'

## example of manual installation into revdep library
## install.packages("rggobi", lib = "~/resources/R/revdep_library/")

## except, for devtools and it's dependencies, I did:
# withr::with_libpaths(
#   "~/resources/R/revdep_library/",
#   devtools::install_github("hadley/devtools"),
#   #source("https://install-github.me/hadley/devtools"),
#   "prefix"
# )
