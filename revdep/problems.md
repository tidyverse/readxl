# aire.zmvm

Version: 0.6.0

## In both

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 52 marked UTF-8 strings
    ```

# BEACH

Version: 1.2.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘readxl’
      All declared Imports should be used.
    ```

# breathtestcore

Version: 0.4.0

## In both

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘breathteststan’
    ```

# cRegulome

Version: 0.1.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘AnnotationDbi’ ‘clusterProfiler’ ‘org.Hs.eg.db’
      All declared Imports should be used.
    ```

# crosswalkr

Version: 0.1.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘dplyr’
      All declared Imports should be used.
    ```

# dynBiplotGUI

Version: 1.1.5

## In both

*   R CMD check timed out
    

# elementR

Version: 1.3.5

## In both

*   checking examples ... ERROR
    ```
    Running examples in ‘elementR-Ex.R’ failed
    The error most likely occurred in:
    
    > ### Name: elementR_project
    > ### Title: Object elementR_project
    > ### Aliases: elementR_project
    > 
    > ### ** Examples
    > 
    > ## create a new elementR_repStandard object based on the "filePath" 
    > ## from a folder containing sample replicate
    > 
    > filePath <- system.file("Example_Session", package="elementR")
    > 
    > exampleProject <- elementR_project$new(filePath)
    Error in structure(.External(.C_dotTclObjv, objv), class = "tclObj") : 
      [tcl] invalid command name "toplevel".
    Calls: <Anonymous> ... tktoplevel -> tkwidget -> tcl -> .Tcl.objv -> structure
    Execution halted
    ```

# GerminaR

Version: 1.2

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘DT’ ‘shinydashboard’
      All declared Imports should be used.
    ```

# GWSDAT

Version: 3.0.0

## In both

*   checking whether package ‘GWSDAT’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/GWSDAT/new/GWSDAT.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘GWSDAT’ ...
** package ‘GWSDAT’ successfully unpacked and MD5 sums checked
** R
** inst
** preparing package for lazy loading
Error in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]) : 
  there is no package called ‘classInt’
ERROR: lazy loading failed for package ‘GWSDAT’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/GWSDAT/new/GWSDAT.Rcheck/GWSDAT’

```
### CRAN

```
* installing *source* package ‘GWSDAT’ ...
** package ‘GWSDAT’ successfully unpacked and MD5 sums checked
** R
** inst
** preparing package for lazy loading
Error in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]) : 
  there is no package called ‘classInt’
ERROR: lazy loading failed for package ‘GWSDAT’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/GWSDAT/old/GWSDAT.Rcheck/GWSDAT’

```
# hiReadsProcessor

Version: 1.12.0

## In both

*   checking for hidden files and directories ... NOTE
    ```
    Found the following hidden files and directories:
      .BBSoptions
    These were most likely included in error. See section ‘Package
    structure’ in the ‘Writing R Extensions’ manual.
    ```

*   checking R code for possible problems ... NOTE
    ```
    ...
      ‘qBaseInsert’
    read.psl : <anonymous>: no visible binding for global variable
      ‘tBaseInsert’
    read.psl: no visible binding for global variable ‘matches’
    read.psl: no visible binding for global variable ‘misMatches’
    read.psl: no visible binding for global variable ‘qBaseInsert’
    read.psl: no visible binding for global variable ‘tBaseInsert’
    read.sampleInfo: no visible global function definition for ‘SimpleList’
    splitSeqsToFiles: no visible global function definition for
      ‘fasta.info’
    vpairwiseAlignSeqs: no visible global function definition for ‘Rle’
    vpairwiseAlignSeqs: no visible global function definition for
      ‘runLength’
    vpairwiseAlignSeqs: no visible global function definition for ‘IRanges’
    vpairwiseAlignSeqs: no visible global function definition for
      ‘runValue’
    Undefined global functions or variables:
      DataFrame IRanges IRangesList Rle ScanBamParam SimpleList
      breakInChunks clusteredValue clusteredValue.freq detectCores
      fasta.info matches mclapply metadata metadata<- misMatches
      qBaseInsert queryHits runLength runValue scanBamFlag tBaseInsert
    ```

# ie2misc

Version: 0.8.5

## In both

*   checking package dependencies ... NOTE
    ```
    Package suggested but not available for checking: ‘ie2miscdata’
    ```

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘moments’
    ```

# memapp

Version: 2.9

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘DT’ ‘RColorBrewer’ ‘RODBC’ ‘dplyr’ ‘formattable’ ‘ggplot2’
      ‘ggthemes’ ‘magrittr’ ‘mem’ ‘openxlsx’ ‘plotly’ ‘readxl’ ‘shinyBS’
      ‘shinydashboard’ ‘shinyjs’ ‘shinythemes’ ‘stringi’ ‘stringr’ ‘tidyr’
      All declared Imports should be used.
    ```

# PCRedux

Version: 0.2.5-1

## In both

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘caret’
    ```

# popprxl

Version: 0.1.3

## In both

*   checking whether package ‘popprxl’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘poppr’ was built under R version 3.4.4
      Warning: package ‘ade4’ was built under R version 3.4.4
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/popprxl/new/popprxl.Rcheck/00install.out’ for details.
    ```

# QuantTools

Version: 0.5.7

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  5.4Mb
      sub-directories of 1Mb or more:
        data   4.5Mb
    ```

# rattle

Version: 5.1.0

## In both

*   checking package dependencies ... ERROR
    ```
    Packages required but not available: ‘RGtk2’ ‘cairoDevice’
    
    Packages suggested but not available for checking:
      ‘gWidgetsRGtk2’ ‘playwith’ ‘rggobi’ ‘RGtk2Extras’
    
    See section ‘The DESCRIPTION file’ in the ‘Writing R Extensions’
    manual.
    ```

# Rcmdr

Version: 2.4-4

## In both

*   checking whether package ‘Rcmdr’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘RcmdrMisc’ was built under R version 3.4.4
      Warning: package ‘car’ was built under R version 3.4.4
      Warning: package ‘carData’ was built under R version 3.4.4
      Warning: package ‘effects’ was built under R version 3.4.4
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/Rcmdr/new/Rcmdr.Rcheck/00install.out’ for details.
    ```

*   checking installed package size ... NOTE
    ```
      installed size is  6.7Mb
      sub-directories of 1Mb or more:
        R     1.0Mb
        doc   3.0Mb
        po    2.1Mb
    ```

# RcmdrMisc

Version: 1.0-10

## In both

*   checking whether package ‘RcmdrMisc’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘car’ was built under R version 3.4.4
      Warning: package ‘carData’ was built under R version 3.4.4
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/RcmdrMisc/new/RcmdrMisc.Rcheck/00install.out’ for details.
    ```

# RCzechia

Version: 1.2.3

## In both

*   checking examples ... ERROR
    ```
    Running examples in ‘RCzechia-Ex.R’ failed
    The error most likely occurred in:
    
    > ### Name: kraje
    > ### Title: Regions (kraje) of the Czech Republic
    > ### Aliases: kraje
    > 
    > ### ** Examples
    > 
    > library(sf)
    Warning: package ‘sf’ was built under R version 3.4.4
    Error: package or namespace load failed for ‘sf’ in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]):
     there is no package called ‘classInt’
    Execution halted
    ```

*   checking tests ...
    ```
     ERROR
    Running the tests in ‘tests/testthat.R’ failed.
    Last 13 lines of output:
             if (logical.return) 
                 message(paste("Error:", msg), domain = NA)
             else stop(msg, call. = FALSE, domain = NA)
         })
      3: tryCatchList(expr, classes, parentenv, handlers)
      4: tryCatchOne(expr, names, parentenv, handlers[[1L]])
      5: value[[3L]](cond)
      6: stop(msg, call. = FALSE, domain = NA)
      
      ══ testthat results  ══════════════════════════════════════════════════
      OK: 1 SKIPPED: 0 FAILED: 1
      1. Error: (unknown) (@test-jla.R#3) 
      
      Error: testthat unit tests failed
      Execution halted
    ```

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Warning in engine$weave(file, quiet = quiet, encoding = enc) :
      The vignette engine knitr::rmarkdown is not available, because the rmarkdown package is not installed. Please install it.
    
    Attaching package: 'dplyr'
    
    The following objects are masked from 'package:stats':
    
        filter, lag
    
    The following objects are masked from 'package:base':
    
        intersect, setdiff, setequal, union
    
    Quitting from lines 27-57 (vignette.Rmd) 
    Error: processing vignette 'vignette.Rmd' failed with diagnostics:
    package or namespace load failed for 'tmap' in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]):
     there is no package called 'classInt'
    Execution halted
    ```

# readtext

Version: 0.50

## In both

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 1 marked Latin-1 string
      Note: found 1 marked UTF-8 string
      Note: found 7 strings marked as "bytes"
    ```

# Ricetl

Version: 0.2.5

## In both

*   checking package dependencies ... ERROR
    ```
    Package required but not available: ‘gWidgetsRGtk2’
    
    See section ‘The DESCRIPTION file’ in the ‘Writing R Extensions’
    manual.
    ```

# shinyHeatmaply

Version: 0.1.0

## In both

*   checking whether package ‘shinyHeatmaply’ can be installed ... WARNING
    ```
    Found the following significant warnings:
      Warning: package ‘viridis’ was built under R version 3.4.4
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/shinyHeatmaply/new/shinyHeatmaply.Rcheck/00install.out’ for details.
    ```

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘RColorBrewer’ ‘htmlwidgets’ ‘jsonlite’ ‘viridis’
      All declared Imports should be used.
    ```

# textreadr

Version: 0.7.0

## In both

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘tm’
    ```

# tidyverse

Version: 1.2.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘dbplyr’ ‘reprex’ ‘rlang’
      All declared Imports should be used.
    ```

# tidyxl

Version: 1.0.1

## In both

*   checking compiled code ... WARNING
    ```
    File ‘tidyxl/libs/tidyxl.so’:
      Found ‘_abort’, possibly from ‘abort’ (C)
        Object: ‘xlex.o’
    
    Compiled code should not call entry points which might terminate R nor
    write to stdout/stderr instead of to the console, nor the system RNG.
    
    See ‘Writing portable packages’ in the ‘Writing R Extensions’ manual.
    ```

# unvotes

Version: 0.2.0

## In both

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 4494 marked UTF-8 strings
    ```

# zooaRchGUI

Version: 1.0.2

## In both

*   checking whether package ‘zooaRchGUI’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/zooaRchGUI/new/zooaRchGUI.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘zooaRchGUI’ ...
** package ‘zooaRchGUI’ successfully unpacked and MD5 sums checked
** R
** data
*** moving datasets to lazyload DB
** inst
** preparing package for lazy loading
Error : .onLoad failed in loadNamespace() for 'rjags', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so, 10): Library not loaded: /usr/local/lib/libjags.4.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so
  Reason: image not found
ERROR: lazy loading failed for package ‘zooaRchGUI’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/zooaRchGUI/new/zooaRchGUI.Rcheck/zooaRchGUI’

```
### CRAN

```
* installing *source* package ‘zooaRchGUI’ ...
** package ‘zooaRchGUI’ successfully unpacked and MD5 sums checked
** R
** data
*** moving datasets to lazyload DB
** inst
** preparing package for lazy loading
Error : .onLoad failed in loadNamespace() for 'rjags', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so, 10): Library not loaded: /usr/local/lib/libjags.4.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so
  Reason: image not found
ERROR: lazy loading failed for package ‘zooaRchGUI’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/zooaRchGUI/old/zooaRchGUI.Rcheck/zooaRchGUI’

```
