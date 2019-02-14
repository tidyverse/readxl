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

*   R CMD check timed out
    

*   checking installed package size ... NOTE
    ```
      installed size is 115.4Mb
      sub-directories of 1Mb or more:
        data     80.0Mb
        doc      26.5Mb
        extdata   8.5Mb
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
    

# crosswalkr

Version: 0.2.2

## In both

*   checking dependencies in R code ... NOTE
    ```
    Namespace in Imports field not imported from: ‘dplyr’
      All declared Imports should be used.
    ```

# DAPAR

Version: 1.14.5

## In both

*   checking examples ... ERROR
    ```
    ...
    > ### Title: creates a column for the protein dataset after agregation by
    > ###   using the previous peptide dataset.
    > ### Aliases: BuildColumnToProteinDataset
    > 
    > ### ** Examples
    > 
    > require(DAPARdata)
    Loading required package: DAPARdata
    
    This is DAPARdata version 1.12.1.
    Use 'DAPARdata()' to list available data sets.
    > data(Exp1_R25_pept)
    > protID <- "Protein.group.IDs"
    > obj.pep <- Exp1_R25_pept[1:1000]
    > M <- BuildAdjacencyMatrix(obj.pep, protID, FALSE)
    > data <- Biobase::fData(obj.pep)
    > protData <- DAPAR::aggregateMean(obj.pep, M)
    Error in installed.packages(lib.loc = lib.loc$Prostar.loc)["Prostar",  : 
      subscript out of bounds
    Calls: <Anonymous> -> finalizeAggregation
    Execution halted
    ```

*   checking R code for possible problems ... NOTE
    ```
    ...
    violinPlotD: warning in axis(side = 1, at = 1:ncol(qData), label = if
      (is.vector(legend)) {: partial argument match of 'label' to 'labels'
    violinPlotD: warning in legend: partial argument match of 'label' to
      'labels'
    violinPlotD: warning in } else {: partial argument match of 'label' to
      'labels'
    violinPlotD: warning in legend[, i]: partial argument match of 'label'
      to 'labels'
    violinPlotD: warning in }, line = 2 * i - 1): partial argument match of
      'label' to 'labels'
    aggregateIterParallel: no visible binding for global variable ‘cond’
    boxPlotD: no visible binding for global variable ‘conds’
    densityPlotD: no visible binding for global variable ‘conds’
    diffAnaVolcanoplot_rCharts: no visible binding for global variable ‘x’
    diffAnaVolcanoplot_rCharts: no visible binding for global variable ‘y’
    diffAnaVolcanoplot_rCharts: no visible binding for global variable ‘g’
    getTextForGOAnalysis: no visible binding for global variable
      ‘textGOParams’
    getTextForGOAnalysis: no visible binding for global variable ‘input’
    Undefined global functions or variables:
      cond conds g input textGOParams x y
    ```

*   checking re-building of vignette outputs ... NOTE
    ```
    Error in re-building vignettes:
      ...
    Error in texi2dvi(file = file, pdf = TRUE, clean = clean, quiet = quiet,  : 
      Running 'texi2dvi' on 'Prostar_UserManual.tex' failed.
    LaTeX errors:
    ! LaTeX Error: File `soul.sty' not found.
    
    Type X to quit or <RETURN> to proceed,
    or enter new name. (Default extension: sty)
    
    ! Emergency stop.
    <read *> 
             
    l.8 \usepackage
                   {url}^^M
    !  ==> Fatal error occurred, no output PDF file produced!
    Calls: buildVignettes -> texi2pdf -> texi2dvi
    Execution halted
    ```

# DataLoader

Version: 1.3

## In both

*   checking whether package ‘DataLoader’ can be installed ... ERROR
    ```
    Installation failed.
    See ‘/Users/max/github/forks/readxl/revdep/checks.noindex/DataLoader/new/DataLoader.Rcheck/00install.out’ for details.
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
  error: unable to load shared object '/Users/max/github/forks/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so':
  dlopen(/Users/max/github/forks/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/max/github/forks/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so
  Reason: image not found
Error : package ‘xlsx’ could not be loaded
ERROR: lazy loading failed for package ‘DataLoader’
* removing ‘/Users/max/github/forks/readxl/revdep/checks.noindex/DataLoader/new/DataLoader.Rcheck/DataLoader’

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
  error: unable to load shared object '/Users/max/github/forks/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so':
  dlopen(/Users/max/github/forks/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/max/github/forks/readxl/revdep/library.noindex/DataLoader/rJava/libs/rJava.so
  Reason: image not found
Error : package ‘xlsx’ could not be loaded
ERROR: lazy loading failed for package ‘DataLoader’
* removing ‘/Users/max/github/forks/readxl/revdep/checks.noindex/DataLoader/old/DataLoader.Rcheck/DataLoader’

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
      installed size is  5.5Mb
      sub-directories of 1Mb or more:
        R      2.0Mb
        data   2.1Mb
    ```

# Doscheda

Version: 1.4.1

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

*   checking dependencies in R code ... NOTE
    ```
    Namespaces in Imports field not imported from:
      ‘DT’ ‘shinydashboard’
      All declared Imports should be used.
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
    See ‘/Users/max/github/forks/readxl/revdep/checks.noindex/ie2misc/new/ie2misc.Rcheck/00install.out’ for details.
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
  error: unable to load shared object '/Users/max/github/forks/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so':
  dlopen(/Users/max/github/forks/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/max/github/forks/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘ie2misc’
* removing ‘/Users/max/github/forks/readxl/revdep/checks.noindex/ie2misc/new/ie2misc.Rcheck/ie2misc’

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
  error: unable to load shared object '/Users/max/github/forks/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so':
  dlopen(/Users/max/github/forks/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/max/github/forks/readxl/revdep/library.noindex/ie2misc/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘ie2misc’
* removing ‘/Users/max/github/forks/readxl/revdep/checks.noindex/ie2misc/old/ie2misc.Rcheck/ie2misc’

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
    See ‘/Users/max/github/forks/readxl/revdep/checks.noindex/joinXL/new/joinXL.Rcheck/00install.out’ for details.
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
  error: unable to load shared object '/Users/max/github/forks/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so':
  dlopen(/Users/max/github/forks/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/max/github/forks/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘joinXL’
* removing ‘/Users/max/github/forks/readxl/revdep/checks.noindex/joinXL/new/joinXL.Rcheck/joinXL’

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
  error: unable to load shared object '/Users/max/github/forks/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so':
  dlopen(/Users/max/github/forks/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so, 6): Library not loaded: /Library/Java/JavaVirtualMachines/jdk-9.jdk/Contents/Home/lib/server/libjvm.dylib
  Referenced from: /Users/max/github/forks/readxl/revdep/library.noindex/joinXL/rJava/libs/rJava.so
  Reason: image not found
ERROR: lazy loading failed for package ‘joinXL’
* removing ‘/Users/max/github/forks/readxl/revdep/checks.noindex/joinXL/old/joinXL.Rcheck/joinXL’

```
# jubilee

Version: 0.2.5

## In both

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Error: processing vignette 'jubilee-tutorial.ltx' failed with diagnostics:
    Running 'texi2dvi' on 'jubilee-tutorial.ltx' failed.
    LaTeX errors:
    ! LaTeX Error: File `float.sty' not found.
    
    Type X to quit or <RETURN> to proceed,
    or enter new name. (Default extension: sty)
    
    ! Emergency stop.
    <read *> 
             
    l.32 \usepackage
                    {amsmath}^^M
    !  ==> Fatal error occurred, no output PDF file produced!
    Execution halted
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

Version: 1.0.1

## In both

*   checking R code for possible problems ... NOTE
    ```
    gplot: no visible binding for global variable ‘max_value’
    gplot: no visible binding for global variable ‘max_error’
    Undefined global functions or variables:
      max_error max_value
    ```

# progeny

Version: 1.4.1

## In both

*   checking package subdirectories ... WARNING
    ```
    Invalid citation information in ‘inst/CITATION’:
      Error in bibentry(bibtype = entry, textVersion = textVersion, header = header,     footer = footer, ...): argument "textVersion" is missing, with no default
    ```

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

*   checking re-building of vignette outputs ... WARNING
    ```
    Error in re-building vignettes:
      ...
    Error in texi2dvi(file = file, pdf = TRUE, clean = clean, quiet = quiet,  : 
      Running 'texi2dvi' on 'rattle.tex' failed.
    LaTeX errors:
    ! LaTeX Error: File `lastpage.sty' not found.
    
    Type X to quit or <RETURN> to proceed,
    or enter new name. (Default extension: sty)
    
    ! Emergency stop.
    <read *> 
             
    l.13 \usepackage
                    {algorithm2e}^^M
    !  ==> Fatal error occurred, no output PDF file produced!
    Calls: buildVignettes -> texi2pdf -> texi2dvi
    Execution halted
    ```

*   checking package dependencies ... NOTE
    ```
    Packages suggested but not available for checking:
      ‘cairoDevice’ ‘gWidgetsRGtk2’ ‘playwith’ ‘rggobi’ ‘RGtk2’
      ‘RGtk2Extras’
    ```

*   checking installed package size ... NOTE
    ```
      installed size is  7.9Mb
      sub-directories of 1Mb or more:
        data   3.1Mb
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

*   checking re-building of vignette outputs ... NOTE
    ```
    ...
    
    The following object is masked from ‘package:car’:
    
        Confint
    
    Error in texi2dvi(file = file, pdf = TRUE, clean = clean, quiet = quiet,  : 
      Running 'texi2dvi' on 'Getting-Started-with-the-Rcmdr.tex' failed.
    LaTeX errors:
    ! LaTeX Error: File `ae.sty' not found.
    
    Type X to quit or <RETURN> to proceed,
    or enter new name. (Default extension: sty)
    
    ! Emergency stop.
    <read *> 
             
    l.30 \ifthenelse
                    {\boolean{Sweave@inconsolata}}{%^^M
    !  ==> Fatal error occurred, no output PDF file produced!
    Calls: buildVignettes -> texi2pdf -> texi2dvi
    Execution halted
    ```

# RCzechia

Version: 1.3.2

## Newly broken

*   R CMD check timed out
    

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

## Newly broken

*   R CMD check timed out
    

## Newly fixed

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
        R         2.1Mb
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
    See ‘/Users/max/github/forks/readxl/revdep/checks.noindex/zooaRchGUI/new/zooaRchGUI.Rcheck/00install.out’ for details.
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
  error: unable to load shared object '/Users/max/github/forks/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so':
  dlopen(/Users/max/github/forks/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so, 10): Library not loaded: /usr/local/lib/libjags.4.dylib
  Referenced from: /Users/max/github/forks/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so
  Reason: image not found
ERROR: lazy loading failed for package ‘zooaRchGUI’
* removing ‘/Users/max/github/forks/readxl/revdep/checks.noindex/zooaRchGUI/new/zooaRchGUI.Rcheck/zooaRchGUI’

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
  error: unable to load shared object '/Users/max/github/forks/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so':
  dlopen(/Users/max/github/forks/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so, 10): Library not loaded: /usr/local/lib/libjags.4.dylib
  Referenced from: /Users/max/github/forks/readxl/revdep/library.noindex/zooaRchGUI/rjags/libs/rjags.so
  Reason: image not found
ERROR: lazy loading failed for package ‘zooaRchGUI’
* removing ‘/Users/max/github/forks/readxl/revdep/checks.noindex/zooaRchGUI/old/zooaRchGUI.Rcheck/zooaRchGUI’

```
