# aire.zmvm

Version: 0.8.0

## In both

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 52 marked UTF-8 strings
    ```

# BEACH

Version: 1.2.1

## In both

*   checking whether package ‘BEACH’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/BEACH/new/BEACH.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘BEACH’ ...
** package ‘BEACH’ successfully unpacked and MD5 sums checked
** R
** inst
** byte-compile and prepare package for lazy loading
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/BEACH/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/BEACH/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/BEACH/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘BEACH’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/BEACH/new/BEACH.Rcheck/BEACH’

```
### CRAN

```
* installing *source* package ‘BEACH’ ...
** package ‘BEACH’ successfully unpacked and MD5 sums checked
** R
** inst
** byte-compile and prepare package for lazy loading
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/BEACH/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/BEACH/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/BEACH/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘BEACH’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/BEACH/old/BEACH.Rcheck/BEACH’

```
# bikedata

Version: 0.2.2

## In both

*   checking tests ...
    ```
     ERROR
    Running the tests in ‘tests/testthat.R’ failed.
    Last 13 lines of output:
         })
      5: tryCatch(if (missE) ...elt(i) else eval(cl.i, envir = envir), error = function(e) {
             e$call <- cl.i
             stop(e)
         })
      6: tryCatchList(expr, classes, parentenv, handlers)
      7: tryCatchOne(expr, names, parentenv, handlers[[1L]])
      8: value[[3L]](cond)
      
      ══ testthat results  ══════════════════════════════════════════════════════════════
      OK: 104 SKIPPED: 4 FAILED: 1
      1. Error: dl_bikedata nyc (@test-download-data.R#39) 
      
      Error: testthat unit tests failed
      Execution halted
    ```

# BloodCancerMultiOmics2017

Version: 1.2.0

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    ...
    
    The following objects are masked from 'package:IRanges':
    
        intersect, setdiff, union
    
    The following objects are masked from 'package:S4Vectors':
    
        intersect, setdiff, union
    
    The following objects are masked from 'package:BiocGenerics':
    
        intersect, setdiff, union
    
    The following objects are masked from 'package:base':
    
        intersect, setdiff, union
    
    Quitting from lines 46-92 (BloodCancerMultiOmics2017.Rmd) 
    Error: processing vignette 'BloodCancerMultiOmics2017.Rmd' failed with diagnostics:
    there is no package called 'org.Hs.eg.db'
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

# cNORM

Version: 1.1.2

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘rmarkdown’
      All declared Imports should be used.
    ```

# cRegulome

Version: 0.2.0

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
Warning in fun(libname, pkgname) :
  mzR has been built against a different Rcpp version (0.12.19)
than is installed on your system (1.0.0). This might lead to errors
when loading mzR. If you encounter such issues, please send a report,
including the output of sessionInfo() to the Bioc support forum at 
https://support.bioconductor.org/. For details see also
https://github.com/sneumann/mzR/wiki/mzR-Rcpp-compiler-linker-issue.
Error in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]) : 
  there is no package called ‘DO.db’
ERROR: lazy loading failed for package ‘DAPAR’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/DAPAR/new/DAPAR.Rcheck/DAPAR’

```
### CRAN

```
* installing *source* package ‘DAPAR’ ...
** R
** inst
** byte-compile and prepare package for lazy loading
Warning in fun(libname, pkgname) :
  mzR has been built against a different Rcpp version (0.12.19)
than is installed on your system (1.0.0). This might lead to errors
when loading mzR. If you encounter such issues, please send a report,
including the output of sessionInfo() to the Bioc support forum at 
https://support.bioconductor.org/. For details see also
https://github.com/sneumann/mzR/wiki/mzR-Rcpp-compiler-linker-issue.
Error in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]) : 
  there is no package called ‘DO.db’
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
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
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
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
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

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    
    Attaching package: 'dplyr'
    
    The following objects are masked from 'package:stats':
    
        filter, lag
    
    The following objects are masked from 'package:base':
    
        intersect, setdiff, setequal, union
    
    Loading required package: RSQLite
    no column `person_id` provided, automatically generating unique person id's
    File img/main_bar.PNG not found in resource path
    Error: processing vignette 'dextergui.Rmd' failed with diagnostics:
    pandoc document conversion failed with error 99
    Execution halted
    ```

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
      installed size is  6.5Mb
      sub-directories of 1Mb or more:
        R      2.1Mb
        data   2.1Mb
        libs   1.4Mb
    ```

# Doscheda

Version: 1.4.0

## In both

*   checking installed package size ... NOTE
    ```
      installed size is  6.0Mb
      sub-directories of 1Mb or more:
        data             2.1Mb
        doc              1.6Mb
        shiny-examples   2.0Mb
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

Version: 3.0.1

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
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
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
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
Error : .onLoad failed in loadNamespace() for 'rJava', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘ie2misc’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/ie2misc/old/ie2misc.Rcheck/ie2misc’

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
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
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
Unable to find any JVMs matching version "(null)".
No Java runtime present, try --request to install.
Warning in system("/usr/libexec/java_home", intern = TRUE) :
  running command '/usr/libexec/java_home' had status 1
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
clang++ -std=gnu++11 -I"/Library/Frameworks/R.framework/Resources/include" -DNDEBUG -I../inst/include/ -I"/Users/jenny/rrr/readxl/revdep/library.noindex/lpirfs/Rcpp/include" -I"/Users/jenny/rrr/readxl/revdep/library.noindex/lpirfs/RcppArmadillo/include" -I/usr/local/include  -fopenmp -fPIC  -Wall -g -O2 -c RcppExports.cpp -o RcppExports.o
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

Version: 2.11

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

*   checking whether package ‘PCRedux’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/PCRedux/new/PCRedux.Rcheck/00install.out’ for details.
    ```

## Installation

### Devel

```
* installing *source* package ‘PCRedux’ ...
** package ‘PCRedux’ successfully unpacked and MD5 sums checked
** R
** data
*** moving datasets to lazyload DB
** inst
** byte-compile and prepare package for lazy loading
Error in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]) : 
  there is no package called ‘quantreg’
ERROR: lazy loading failed for package ‘PCRedux’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/PCRedux/new/PCRedux.Rcheck/PCRedux’

```
### CRAN

```
* installing *source* package ‘PCRedux’ ...
** package ‘PCRedux’ successfully unpacked and MD5 sums checked
** R
** data
*** moving datasets to lazyload DB
** inst
** byte-compile and prepare package for lazy loading
Error in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]) : 
  there is no package called ‘quantreg’
ERROR: lazy loading failed for package ‘PCRedux’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/PCRedux/old/PCRedux.Rcheck/PCRedux’

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

# progeny

Version: 1.4.0

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Quitting from lines 42-60 (progeny.Rmd) 
    Error: processing vignette 'progeny.Rmd' failed with diagnostics:
    there is no package called 'airway'
    Execution halted
    ```

*   checking package dependencies ... NOTE
    ```
    Package suggested but not available for checking: ‘airway’
    ```

# ProjectTemplate

Version: 0.8.2

## In both

*   checking dependencies in R code ... NOTE
    ```
    Unable to find any JVMs matching version "(null)".
    No Java runtime present, try --request to install.
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

# RCzechia

Version: 1.3.1

## In both

*   checking tests ...
    ```
     ERROR
    Running the tests in ‘tests/testthat.R’ failed.
    Last 13 lines of output:
                 message(paste("Error:", msg), domain = NA)
             else stop(msg, call. = FALSE, domain = NA)
         })
      3: tryCatchList(expr, classes, parentenv, handlers)
      4: tryCatchOne(expr, names, parentenv, handlers[[1L]])
      5: value[[3L]](cond)
      6: stop(msg, call. = FALSE, domain = NA)
      
      ══ testthat results  ════════════════════════════════════════════════════════════════════════
      OK: 0 SKIPPED: 0 FAILED: 2
      1. Error: (unknown) (@test-1-data-structures.R#3) 
      2. Error: (unknown) (@test-2-code.R#2) 
      
      Error: testthat unit tests failed
      Execution halted
    ```

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    
    Attaching package: 'dplyr'
    
    The following objects are masked from 'package:stats':
    
        filter, lag
    
    The following objects are masked from 'package:base':
    
        intersect, setdiff, setequal, union
    
    Quitting from lines 28-58 (vignette.Rmd) 
    Error: processing vignette 'vignette.Rmd' failed with diagnostics:
    package or namespace load failed for 'tmap' in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]):
     there is no package called 'classInt'
    Execution halted
    ```

# RDML

Version: 0.9-9

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Quitting from lines 42-47 (CreateRDML.Rmd) 
    Error: processing vignette 'CreateRDML.Rmd' failed with diagnostics:
    package or namespace load failed for 'chipPCR' in loadNamespace(j <- i[[1L]], c(lib.loc, .libPaths()), versionCheck = vI[[j]]):
     there is no package called 'quantreg'
    Execution halted
    ```

*   checking installed package size ... NOTE
    ```
      installed size is  5.5Mb
      sub-directories of 1Mb or more:
        R     2.0Mb
        doc   2.4Mb
    ```

# readabs

Version: 0.2.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘sjmisc’ ‘stringr’
      All declared Imports should be used.
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
    Package required but not available: ‘gWidgetsRGtk2’
    
    See section ‘The DESCRIPTION file’ in the ‘Writing R Extensions’
    manual.
    ```

# RLumShiny

Version: 0.2.1

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘DT’ ‘knitr’
      All declared Imports should be used.
    ```

# secr

Version: 3.1.8

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

Version: 1.0.3

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
** byte-compile and prepare package for lazy loading
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
** byte-compile and prepare package for lazy loading
Error : .onLoad failed in loadNamespace() for 'rjags', details:
  call: dyn.load(file, DLLpath = DLLpath, ...)
  error: unable to load shared object '/Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so':
  dlopen(/Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so, 10): Library not loaded: /usr/local/lib/libjags.4.dylib
  Referenced from: /Users/jenny/rrr/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so
  Reason: image not found
ERROR: lazy loading failed for package ‘zooaRchGUI’
* removing ‘/Users/jenny/rrr/readxl/revdep/checks.noindex/zooaRchGUI/old/zooaRchGUI.Rcheck/zooaRchGUI’

```
