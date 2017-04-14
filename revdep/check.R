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
##  * rggobi nope nope did not install :( I came back to this and
##    did as it says at https://github.com/ggobi/rggobi
##    failed again
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
## except, for devtools and it's dependencies, I did this to get dev versions:
# withr::with_libpaths(
#   "~/resources/R/revdep_library/",
#   devtools::install_github("hadley/devtools"),
#   #source("https://install-github.me/hadley/devtools"),
#   "prefix"
# )

# for doing one-off check of some packages
# revdeps <- revdep("readxl")
# keep <- "DataLoader"
# ignore <- setdiff(revdeps, keep)

ignore <- c(
  "DataLoader",   ## could never check via revdep_check() due to Java problems
                  ## downloaded source from GitHub CRAN mirror and ran
                  ## check in RStudio, with success
  "dynBiplotGUI", ## success w/ R CMD check dynBiplotGUI_1.1.5.tar.gz, but
                  ## various graphical things pop up
  "ie2misc",      ## downloaded source from GitHub CRAN mirror and ran
                  ## check in RStudio, with success
  "SchemaOnRead"  ## downloaded source from GitHub CRAN mirror and ran
                  ## check in RStudio, with success
)

revdep_check(ignore = ignore)
revdep_check_save_summary()
revdep_check_print_problems()

## very helpful for compiling the packages that contain fortran
## http://stackoverflow.com/questions/23916219/os-x-package-installation-depends-on-gfortran-4-8

## very helpful for getting ie2misc to install
## http://stackoverflow.com/questions/35179151/cannot-load-r-xlsx-package-on-mac-os-10-11

## installed GTK+ from here to get rattle to (partially) check
## https://r.research.att.com
## rggobi and RODBC are in Suggests, so full check not possible

## http://stackoverflow.com/questions/30738974/rjava-load-error-in-rstudio-r-after-upgrading-to-osx-yosemite
## sudo ln -f -s $(/usr/libexec/java_home)/jre/lib/server/libjvm.dylib /usr/local/lib
