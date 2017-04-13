library("devtools")

## how I run THIS script from shell:
## R --vanilla -e '.libPaths("~/resources/R/revdep_library");options(devtools.revdep.libpath = "~/resources/R/revdep_library")' -e 'source("revdep/check.R")'

## things did not go smoothly because devtools/callr/pkgbuild are going through
## an awkward phase
## one of the problems is that *everything* is being installed from source
## and, for some reason, that was not generally working via `revdep_check()`,
## in the revdep library
## I ended up doing alot of manual installation

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
##  * RODBC nope did not install :( come back to this?
##  * rggobi nope nope did not install :( came back to this
##    did as it says at https://github.com/ggobi/rggobi
##    failed
##    installed pkgconfig, set PKG_CONFIG_PATH
##    progress, but still failing
##    came quite close, but gave up at this point:
##    ggobi.c:7:10: fatal error: 'gdk/gdkx.h' file not found
##    #include <gdk/gdkx.h>
##             ^
##
## example of manual installation into revdep library
## install.packages("rggobi", lib = "~/resources/R/revdep_library/")
##
## except, for devtools and it's dependencies, I did:
# withr::with_libpaths(
#   "~/resources/R/revdep_library/",
#   devtools::install_github("hadley/devtools"),
#   #source("https://install-github.me/hadley/devtools"),
#   "prefix"
# )

ignore <- c(
  "dynBiplotGUI", ## hangs: * checking whether package ‘dynBiplotGUI’ can be installed ...
  "ie2misc" ## java something something java something
)
revdep_check(ignore = ignore)
revdep_check_save_summary()
revdep_check_print_problems()

## very helpful for compiling the packages that contain fortran
## http://stackoverflow.com/questions/23916219/os-x-package-installation-depends-on-gfortran-4-8

## very helpful for getting ie2misc to install
## http://stackoverflow.com/questions/35179151/cannot-load-r-xlsx-package-on-mac-os-10-11

## manually did
## R CMD check dynBiplotGUI_1.1.5.tar.gz
## and it was OK
## various widgets/menus popup during the check, which presumably explains
## why the check hung inside `revdep_check()` (??)
