# AdhereR

Version: 0.4.1

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  5.2Mb
      sub-directories of 1Mb or more:
        doc   3.2Mb
    ```

# aire.zmvm

Version: 0.8.1

## In both

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 52 marked UTF-8 strings
    ```

# BloodCancerMultiOmics2017

Version: 1.2.0

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    ...
    ✔ readr   1.3.1     ✔ stringr 1.4.0
    ✔ tibble  2.0.1     ✔ forcats 0.3.0
    ── Conflicts ───────────────────────────────────── tidyverse_conflicts() ──
    ✖ ggplot2::Position() masks BiocGenerics::Position(), base::Position()
    ✖ dplyr::collapse()   masks IRanges::collapse()
    ✖ dplyr::combine()    masks Biobase::combine(), BiocGenerics::combine()
    ✖ dplyr::count()      masks matrixStats::count()
    ✖ dplyr::desc()       masks IRanges::desc()
    ✖ tidyr::expand()     masks S4Vectors::expand()
    ✖ dplyr::filter()     masks stats::filter()
    ✖ dplyr::first()      masks S4Vectors::first()
    ✖ dplyr::lag()        masks stats::lag()
    ✖ purrr::reduce()     masks GenomicRanges::reduce(), IRanges::reduce()
    ✖ dplyr::rename()     masks S4Vectors::rename()
    ✖ purrr::simplify()   masks DelayedArray::simplify()
    ✖ dplyr::slice()      masks IRanges::slice()
    Quitting from lines 16-25 (BloodCancerMultiOmics2017-dataOverview.Rmd) 
    Error: processing vignette 'BloodCancerMultiOmics2017-dataOverview.Rmd' failed with diagnostics:
    package or namespace load failed for 'BiocStyle' in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]):
     there is no package called 'BiocManager'
    Execution halted
    ```

*   checking package dependencies ... NOTE
    ```
    Package suggested but not available for checking: ‘org.Hs.eg.db’
    ```

*   checking installed package size ... NOTE
    ```
      installed size is 115.3Mb
      sub-directories of 1Mb or more:
        data     80.0Mb
        doc      26.5Mb
        extdata   8.5Mb
    ```

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘vsn’
    ```

# blorr

Version: 0.2.0

## In both

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘lmtest’
    ```

# breathtestcore

Version: 0.4.6

## In both

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘breathteststan’
    ```

# chillR

Version: 0.70.12

## In both

*   R CMD check timed out
    

# cRegulome

Version: 0.3.0

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    ...
    
    Loading required package: IRanges
    Loading required package: S4Vectors
    
    Attaching package: 'S4Vectors'
    
    The following object is masked from 'package:base':
    
        expand.grid
    
    
    Attaching package: 'IRanges'
    
    The following object is masked from 'package:R.oo':
    
        trim
    
    Quitting from lines 32-41 (case_study.Rmd) 
    Error: processing vignette 'case_study.Rmd' failed with diagnostics:
    there is no package called 'org.Hs.eg.db'
    Execution halted
    ```

*   checking package dependencies ... NOTE
    ```
    Package suggested but not available for checking: ‘org.Hs.eg.db’
    ```

# crosswalkr

Version: 0.2.2

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘dplyr’
      All declared Imports should be used.
    ```

# DAPAR

Version: 1.14.3

## In both

*   checking whether package ‘DAPAR’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/DAPAR/new/DAPAR.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘DAPAR’ ...
** R
** inst
** byte-compile and prepare package for lazy loading
Error in loadNamespace(i, c(lib.loc, .libPaths()), versionCheck = vI[[i]]) : 
  there is no package called ‘ncdf4’
ERROR: lazy loading failed for package ‘DAPAR’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/DAPAR/new/DAPAR.Rcheck/DAPAR’

```
### CRAN

```
* installing *source* package ‘DAPAR’ ...
** R
** inst
** byte-compile and prepare package for lazy loading
Error in loadNamespace(i, c(lib.loc, .libPaths()), versionCheck = vI[[i]]) : 
  there is no package called ‘ncdf4’
ERROR: lazy loading failed for package ‘DAPAR’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/DAPAR/old/DAPAR.Rcheck/DAPAR’

```
# DataLoader

Version: 1.3

## In both

*   checking whether package ‘DataLoader’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/DataLoader/new/DataLoader.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘DataLoader’ ...
** package ‘DataLoader’ successfully unpacked and MD5 sums checked
** R
** byte-compile and prepare package for lazy loading
Error: package or namespace load failed for ‘xlsx’:
 .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so
  Reason: image not found
Error : package ‘xlsx’ could not be loaded
ERROR: lazy loading failed for package ‘DataLoader’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/DataLoader/new/DataLoader.Rcheck/DataLoader’

```
### CRAN

```
* installing *source* package ‘DataLoader’ ...
** package ‘DataLoader’ successfully unpacked and MD5 sums checked
** R
** byte-compile and prepare package for lazy loading
Error: package or namespace load failed for ‘xlsx’:
 .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so
  Reason: image not found
Error : package ‘xlsx’ could not be loaded
ERROR: lazy loading failed for package ‘DataLoader’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/DataLoader/old/DataLoader.Rcheck/DataLoader’

```
# dextergui

Version: 0.1.5

## In both

*   checking dependencies in R code ... NOTE
    ```
    Unexported objects imported by ':::' calls:
      ‘dexter:::get_resp_data’ ‘dexter:::qcolors’
      See the note in ?`:::` about the use of this operator.
    ```

# DLMtool

Version: 5.2.3

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  5.5Mb
      sub-directories of 1Mb or more:
        R      2.0Mb
        data   2.1Mb
    ```

# Doscheda

Version: 1.4.0

## In both

*   checking whether package ‘Doscheda’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/Doscheda/new/Doscheda.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘Doscheda’ ...
** R
** data
*** moving datasets to lazyload DB
** inst
** byte-compile and prepare package for lazy loading
Error in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]) : 
  there is no package called ‘BiocManager’
ERROR: lazy loading failed for package ‘Doscheda’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/Doscheda/new/Doscheda.Rcheck/Doscheda’

```
### CRAN

```
* installing *source* package ‘Doscheda’ ...
** R
** data
*** moving datasets to lazyload DB
** inst
** byte-compile and prepare package for lazy loading
Error in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]) : 
  there is no package called ‘BiocManager’
ERROR: lazy loading failed for package ‘Doscheda’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/Doscheda/old/Doscheda.Rcheck/Doscheda’

```
# duawranglr

Version: 0.6.3

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘digest’ ‘dplyr’
      All declared Imports should be used.
    ```

# dynBiplotGUI

Version: 1.1.5

## In both

*   R CMD check timed out
    

# elementR

Version: 1.3.6

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

# ezpickr

Version: 1.0.0

## In both

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘BrailleR’
    ```

# GerminaR

Version: 1.2

## In both

*   checking whether package ‘GerminaR’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/GerminaR/new/GerminaR.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘GerminaR’ ...
** package ‘GerminaR’ successfully unpacked and MD5 sums checked
** R
** data
*** moving datasets to lazyload DB
** inst
** byte-compile and prepare package for lazy loading
Error in loadNamespace(i, c(lib.loc, .libPaths()), versionCheck = vI[[i]]) : 
  there is no package called ‘spdep’
ERROR: lazy loading failed for package ‘GerminaR’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/GerminaR/new/GerminaR.Rcheck/GerminaR’

```
### CRAN

```
* installing *source* package ‘GerminaR’ ...
** package ‘GerminaR’ successfully unpacked and MD5 sums checked
** R
** data
*** moving datasets to lazyload DB
** inst
** byte-compile and prepare package for lazy loading
Error in loadNamespace(i, c(lib.loc, .libPaths()), versionCheck = vI[[i]]) : 
  there is no package called ‘spdep’
ERROR: lazy loading failed for package ‘GerminaR’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/GerminaR/old/GerminaR.Rcheck/GerminaR’

```
# GWSDAT

Version: 3.0.3

## In both

*   checking package dependencies ... ERROR
    ```
    Package required but not available: ‘sm’
    
    See section ‘The DESCRIPTION file’ in the ‘Writing R Extensions’
    manual.
    ```

# hiReadsProcessor

Version: 1.18.0

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

*   checking whether package ‘ie2misc’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/ie2misc/new/ie2misc.Rcheck/00install.out’ for details.
    ```

*   checking package dependencies ... NOTE
    ```
    Package suggested but not available for checking: ‘ie2miscdata’
    ```

## Installation

### Devel

```
* installing *source* package ‘ie2misc’ ...
** package ‘ie2misc’ successfully unpacked and MD5 sums checked
** R
** inst
** byte-compile and prepare package for lazy loading
Warning in fun(libname, pkgname) : couldn't connect to display ""
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘ie2misc’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/ie2misc/new/ie2misc.Rcheck/ie2misc’

```
### CRAN

```
* installing *source* package ‘ie2misc’ ...
** package ‘ie2misc’ successfully unpacked and MD5 sums checked
** R
** inst
** byte-compile and prepare package for lazy loading
Warning in fun(libname, pkgname) : couldn't connect to display ""
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘ie2misc’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/ie2misc/old/ie2misc.Rcheck/ie2misc’

```
# iotables

Version: 0.4.2

## In both

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 53206 marked UTF-8 strings
    ```

# IsoCorrectoR

Version: 1.0.5

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Error: processing vignette 'IsoCorrectoR.Rmd' failed with diagnostics:
    there is no package called ‘BiocStyle’
    Execution halted
    ```

*   checking installed package size ... NOTE
    ```
      installed size is  6.8Mb
      sub-directories of 1Mb or more:
        testdata   5.2Mb
    ```

# joinXL

Version: 1.0.1

## In both

*   checking whether package ‘joinXL’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/joinXL/new/joinXL.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘joinXL’ ...
** package ‘joinXL’ successfully unpacked and MD5 sums checked
** R
** inst
** byte-compile and prepare package for lazy loading
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘joinXL’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/joinXL/new/joinXL.Rcheck/joinXL’

```
### CRAN

```
* installing *source* package ‘joinXL’ ...
** package ‘joinXL’ successfully unpacked and MD5 sums checked
** R
** inst
** byte-compile and prepare package for lazy loading
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘joinXL’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/joinXL/old/joinXL.Rcheck/joinXL’

```
# lpirfs

Version: 0.1.4

## In both

*   checking whether package ‘lpirfs’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/lpirfs/new/lpirfs.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘lpirfs’ ...
** package ‘lpirfs’ successfully unpacked and MD5 sums checked
** libs
clang++ -std=gnu++11 -I"/Library/Frameworks/R.framework/Resources/include" -DNDEBUG -I../inst/include/ -I"/Users/jenny/rrr/readxl/revdep/library.noindex/readxl/new/Rcpp/include" -I"/Users/jenny/rrr/readxl/revdep/library.noindex/lpirfs/RcppArmadillo/include" -I/usr/local/include  -fopenmp -fPIC  -Wall -g -O2 -c RcppExports.cpp -o RcppExports.o
clang: error: unsupported option '-fopenmp'
make: *** [RcppExports.o] Error 1
ERROR: compilation failed for package ‘lpirfs’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/lpirfs/new/lpirfs.Rcheck/lpirfs’

```
### CRAN

```
* installing *source* package ‘lpirfs’ ...
** package ‘lpirfs’ successfully unpacked and MD5 sums checked
** libs
clang++ -std=gnu++11 -I"/Library/Frameworks/R.framework/Resources/include" -DNDEBUG -I../inst/include/ -I"/Users/jenny/rrr/readxl/revdep/library.noindex/readxl/old/Rcpp/include" -I"/Users/jenny/rrr/readxl/revdep/library.noindex/lpirfs/RcppArmadillo/include" -I/usr/local/include  -fopenmp -fPIC  -Wall -g -O2 -c RcppExports.cpp -o RcppExports.o
clang: error: unsupported option '-fopenmp'
make: *** [RcppExports.o] Error 1
ERROR: compilation failed for package ‘lpirfs’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/lpirfs/old/lpirfs.Rcheck/lpirfs’

```
# manifestoR

Version: 1.3.0

## In both

*   checking R code for possible problems ... NOTE
    ```
    mp_corpus: no visible binding for global variable ‘annotations’
    print.ManifestoAvailability: no visible binding for global variable
      ‘annotations’
    Undefined global functions or variables:
      annotations
    ```

# memapp

Version: 2.12

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘DT’ ‘RColorBrewer’ ‘RODBC’ ‘dplyr’ ‘foreign’ ‘formattable’ ‘ggplot2’
      ‘haven’ ‘mem’ ‘openxlsx’ ‘plotly’ ‘readxl’ ‘shinyBS’ ‘shinydashboard’
      ‘shinydashboardPlus’ ‘shinyjs’ ‘shinythemes’ ‘stringi’ ‘stringr’
      ‘tidyr’
      All declared Imports should be used.
    ```

# PCRedux

Version: 0.2.6-4

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘caret’
      All declared Imports should be used.
    ```

# plethem

Version: 0.1.7

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘V8’ ‘devtools’ ‘formatR’ ‘gdata’ ‘rhandsontable’ ‘shinythemes’
      ‘sqldf’
      All declared Imports should be used.
    ```

# plotGrouper

Version: 1.0.0

## In both

*   checking R code for possible problems ... NOTE
    ```
    gplot: no visible binding for global variable ‘max_value’
    gplot: no visible binding for global variable ‘max_error’
    Undefined global functions or variables:
      max_error max_value
    ```

# popprxl

Version: 0.1.4

## In both

*   checking whether package ‘popprxl’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/popprxl/new/popprxl.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘popprxl’ ...
** package ‘popprxl’ successfully unpacked and MD5 sums checked
** R
** inst
** byte-compile and prepare package for lazy loading
Error: package or namespace load failed for ‘adegenet’ in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]):
 there is no package called ‘igraph’
Error : package ‘adegenet’ could not be loaded
ERROR: lazy loading failed for package ‘popprxl’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/popprxl/new/popprxl.Rcheck/popprxl’

```
### CRAN

```
* installing *source* package ‘popprxl’ ...
** package ‘popprxl’ successfully unpacked and MD5 sums checked
** R
** inst
** byte-compile and prepare package for lazy loading
Error: package or namespace load failed for ‘adegenet’ in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]):
 there is no package called ‘igraph’
Error : package ‘adegenet’ could not be loaded
ERROR: lazy loading failed for package ‘popprxl’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/popprxl/old/popprxl.Rcheck/popprxl’

```
# progeny

Version: 1.4.0

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Warning in engine$weave(file, quiet = quiet, encoding = enc) :
      The vignette engine knitr::rmarkdown is not available, because the rmarkdown package is not installed. Please install it.
    Quitting from lines 42-60 (progeny.Rmd) 
    Error: processing vignette 'progeny.Rmd' failed with diagnostics:
    there is no package called 'airway'
    Execution halted
    ```

*   checking package dependencies ... NOTE
    ```
    Package suggested but not available for checking: ‘airway’
    ```

# QuantTools

Version: 0.5.7

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  5.6Mb
      sub-directories of 1Mb or more:
        data   4.5Mb
    ```

# radiant.data

Version: 0.9.7

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘shinyFiles’
      All declared Imports should be used.
    ```

# rattle

Version: 5.2.0

## In both

*   checking package dependencies ... NOTE
    ```
    Packages suggested but not available for checking:
      ‘cairoDevice’ ‘gWidgetsRGtk2’ ‘playwith’ ‘rggobi’ ‘RGtk2’
      ‘RGtk2Extras’
    ```

*   checking installed package size ... NOTE
    ```
      installed size is  7.8Mb
      sub-directories of 1Mb or more:
        data   3.0Mb
        etc    1.9Mb
        po     1.2Mb
    ```

# Rcmdr

Version: 2.5-1

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  7.7Mb
      sub-directories of 1Mb or more:
        R     2.0Mb
        doc   3.0Mb
        po    2.1Mb
    ```

# RDML

Version: 0.9-9

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  5.5Mb
      sub-directories of 1Mb or more:
        R     2.0Mb
        doc   2.4Mb
    ```

# readtext

Version: 0.71

## In both

*   checking Rd cross-references ... NOTE
    ```
    Package unavailable to check Rd xrefs: ‘XML’
    ```

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
    Packages required but not available: ‘devtools’ ‘gWidgetsRGtk2’
    
    See section ‘The DESCRIPTION file’ in the ‘Writing R Extensions’
    manual.
    ```

# RLumShiny

Version: 0.2.2

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘RCarb’ ‘rmarkdown’ ‘shinyjs’
      All declared Imports should be used.
    ```

# rotl

Version: 3.0.6

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Quitting from lines 98-100 (meta-analysis.Rmd) 
    Error: processing vignette 'meta-analysis.Rmd' failed with diagnostics:
    HTTP failure: 502
    <!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
    <html><head>
    <title>502 Proxy Error</title>
    </head><body>
    <h1>Proxy Error</h1>
    <p>The proxy server received an invalid
    response from an upstream server.<br />
    The proxy server could not handle the request <em><a href="/v3/tnrs/match_names">POST&nbsp;/v3/tnrs/match_names</a></em>.<p>
    Reason: <strong>Error reading from remote server</strong></p></p>
    <hr>
    <address>Apache/2.4.10 (Debian) Server at api.opentreeoflife.org Port 443</address>
    </body></html>
    Execution halted
    ```

# secr

Version: 3.2.0

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  5.6Mb
      sub-directories of 1Mb or more:
        R      2.0Mb
        data   2.0Mb
    ```

# shinyHeatmaply

Version: 0.1.0

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘RColorBrewer’ ‘htmlwidgets’ ‘jsonlite’ ‘viridis’
      All declared Imports should be used.
    ```

# sstModel

Version: 1.0.0

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘shinydashboard’
      All declared Imports should be used.
    ```

# syuzhet

Version: 1.0.4

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  5.8Mb
      sub-directories of 1Mb or more:
        R         2.0Mb
        extdata   3.1Mb
    ```

# TCGAbiolinksGUI.data

Version: 1.2.0

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Error: processing vignette 'vignettes.Rmd' failed with diagnostics:
    there is no package called ‘BiocManager’
    Execution halted
    ```

*   checking installed package size ... NOTE
    ```
      installed size is 19.7Mb
      sub-directories of 1Mb or more:
        data  18.6Mb
        doc    1.0Mb
    ```

# textreadr

Version: 0.9.0

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

Version: 1.0.4

## In both

*   checking compiled code ... WARNING
    ```
    File ‘tidyxl/libs/tidyxl.so’:
      Found ‘_abort’, possibly from ‘abort’ (C)
        Object: ‘xlex.o’
    
    Compiled code should not call entry points which might terminate R nor
    write to stdout/stderr instead of to the console, nor use Fortran I/O
    nor system RNGs.
    
    See ‘Writing portable packages’ in the ‘Writing R Extensions’ manual.
    ```

# TR8

Version: 0.9.19

## In both

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 153 marked Latin-1 strings
      Note: found 108 marked UTF-8 strings
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

*   checking package dependencies ... ERROR
    ```
    Package required but not available: ‘spdep’
    
    See section ‘The DESCRIPTION file’ in the ‘Writing R Extensions’
    manual.
    ```

