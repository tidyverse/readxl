# Setup

## Platform

|setting  |value                        |
|:--------|:----------------------------|
|version  |R version 3.3.2 (2016-10-31) |
|system   |x86_64, darwin13.4.0         |
|ui       |X11                          |
|language |(EN)                         |
|collate  |en_CA.UTF-8                  |
|tz       |America/Vancouver            |
|date     |2017-04-13                   |

## Packages

|package    |*  |version    |date       |source                           |
|:----------|:--|:----------|:----------|:--------------------------------|
|cellranger |   |1.1.0      |2016-07-27 |cran (@1.1.0)                    |
|covr       |   |2.2.2      |2017-01-05 |cran (@2.2.2)                    |
|Rcpp       |   |0.12.10    |2017-04-13 |Github (RcppCore/Rcpp@21d8388)   |
|readxl     |   |0.1.1.9000 |2017-04-13 |local (jennybc/readxl@NA)        |
|rprojroot  |   |1.2        |2017-01-16 |cran (@1.2)                      |
|testthat   |   |1.0.2.9000 |2017-04-13 |Github (hadley/testthat@b72a228) |
|tibble     |   |1.3.0      |2017-04-01 |cran (@1.3.0)                    |

# Check results

5 packages with problems

|package      |version | errors| warnings| notes|
|:------------|:-------|------:|--------:|-----:|
|DataLoader   |1.3     |      1|        0|     0|
|elementR     |1.3.0   |      1|        0|     0|
|heemod       |0.9.0   |      1|        0|     0|
|rattle       |4.1.0   |      1|        5|     4|
|SchemaOnRead |1.0.2   |      1|        1|     0|

## DataLoader (1.3)
Maintainer: Srivenkatesh Gandhi <srivenkateshg@sase.ssn.edu.in>

1 error  | 0 warnings | 0 notes

```
checking whether package ‘DataLoader’ can be installed ... ERROR
Installation failed.
See ‘/Users/jenny/rrr/readxl/revdep/checks/DataLoader.Rcheck/00install.out’ for details.
```

## elementR (1.3.0)
Maintainer: Charlotte Sirot <charlott.sirot@gmail.com>  
Bug reports: https://github.com/charlottesirot/elementR/issues

1 error  | 0 warnings | 0 notes

```
checking examples ... ERROR
Running examples in ‘elementR-Ex.R’ failed
The error most likely occurred in:

> base::assign(".ptime", proc.time(), pos = "CheckExEnv")
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

## heemod (0.9.0)
Maintainer: Antoine Filipovic-Pierucci <pierucci@gmail.com>  
Bug reports: https://github.com/pierucci/heemod/issues

1 error  | 0 warnings | 0 notes

```
checking tests ... ERROR
Running the tests in ‘tests/testthat.R’ failed.
Last 13 lines of output:
  Regional mortality rates cannot be estimated from local data.
  1: get_who_mr(age = 0:99, region = "GLOBAL", year = 2015) at testthat/test_gho.R:35
  2: withVisible(`_f`(age = age, sex = sex, region = region, country = country, year = year, 
         local = local))
  3: `_f`(age = age, sex = sex, region = region, country = country, year = year, local = local)
  4: stop("Regional mortality rates cannot be estimated from local data.")
  
  testthat results ================================================================
  OK: 378 SKIPPED: 0 FAILED: 1
  1. Error: GHO API (@test_gho.R#35) 
  
  Error: testthat unit tests failed
  Execution halted
```

## rattle (4.1.0)
Maintainer: Graham Williams <Graham.Williams@togaware.com>

1 error  | 5 warnings | 4 notes

```
checking whether the package can be loaded ... ERROR
Error in dyn.load(file, DLLpath = DLLpath, ...) : 
  unable to load shared object '/Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so':
  dlopen(/Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so, 6): Library not loaded: /Library/Frameworks/GTK+.framework/Versions/2.24.X11/Resources/lib/libgtk-x11-2.0.0.dylib
  Referenced from: /Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so
  Reason: image not found
Please install GTK+ from http://r.research.att.com/libs/GTK_2.24.17-X11.pkg
If the package still does not load, please ensure that GTK+ is installed and that it is on your PATH environment variable
IN ANY CASE, RESTART R BEFORE TRYING TO LOAD THE PACKAGE AGAIN
Rattle: A free graphical interface for data mining with R.
Version 4.1.0 Copyright (c) 2006-2015 Togaware Pty Ltd.
Type 'rattle()' to shake, rattle, and roll your data.
Warning message:
Failed to load RGtk2 dynamic library, attempting to install it. 

It looks like this package has a loading problem: see the messages for
details.

checking whether package ‘rattle’ can be installed ... WARNING
Found the following significant warnings:
  Warning: Failed to load RGtk2 dynamic library, attempting to install it.
See ‘/Users/jenny/rrr/readxl/revdep/checks/rattle.Rcheck/00install.out’ for details.

checking whether the package can be unloaded cleanly ... WARNING
---- unloading

checking whether the namespace can be loaded with stated dependencies ... WARNING
Error in dyn.load(file, DLLpath = DLLpath, ...) : 
  unable to load shared object '/Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so':
  dlopen(/Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so, 6): Library not loaded: /Library/Frameworks/GTK+.framework/Versions/2.24.X11/Resources/lib/libgtk-x11-2.0.0.dylib
  Referenced from: /Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so
  Reason: image not found

A namespace must be able to be loaded with just the base namespace
loaded: otherwise if the namespace gets loaded by a saved object, the
session will be unable to start.

Probably some imports need to be declared in the NAMESPACE file.

checking whether the namespace can be unloaded cleanly ... WARNING
---- unloading

checking loading without being on the library search path ... WARNING
Error in dyn.load(file, DLLpath = DLLpath, ...) : 
  unable to load shared object '/Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so':
  dlopen(/Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so, 6): Library not loaded: /Library/Frameworks/GTK+.framework/Versions/2.24.X11/Resources/lib/libgtk-x11-2.0.0.dylib
  Referenced from: /Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so
  Reason: image not found
Please install GTK+ from http://r.research.att.com/libs/GTK_2.24.17-X11.pkg
If the package still does not load, please ensure that GTK+ is installed and that it is on your PATH environment variable
IN ANY CASE, RESTART R BEFORE TRYING TO LOAD THE PACKAGE AGAIN
Rattle: A free graphical interface for data mining with R.
Version 4.1.0 Copyright (c) 2006-2015 Togaware Pty Ltd.
Type 'rattle()' to shake, rattle, and roll your data.
Warning message:
Failed to load RGtk2 dynamic library, attempting to install it. 

It looks like this package has a loading problem when not on .libPaths:
see the messages for details.

checking package dependencies ... NOTE
Packages suggested but not available for checking:
  ‘graph’ ‘RBGL’ ‘rggobi’ ‘RODBC’ ‘pkgDepTools’ ‘Rgraphviz’

checking installed package size ... NOTE
  installed size is  6.8Mb
  sub-directories of 1Mb or more:
    data   2.5Mb
    etc    1.9Mb
    po     1.2Mb

checking dependencies in R code ... NOTE
Error in dyn.load(file, DLLpath = DLLpath, ...) : 
  unable to load shared object '/Users/jenny/resources/R/revdep_library/cairoDevice/libs/cairoDevice.so':
  dlopen(/Users/jenny/resources/R/revdep_library/cairoDevice/libs/cairoDevice.so, 6): Library not loaded: /Library/Frameworks/GTK+.framework/Versions/2.24.X11/Resources/lib/libgtk-x11-2.0.0.dylib
  Referenced from: /Users/jenny/resources/R/revdep_library/cairoDevice/libs/cairoDevice.so
  Reason: image not found

checking R code for possible problems ... NOTE
Error in dyn.load(file, DLLpath = DLLpath, ...) : 
  unable to load shared object '/Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so':
  dlopen(/Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so, 6): Library not loaded: /Library/Frameworks/GTK+.framework/Versions/2.24.X11/Resources/lib/libgtk-x11-2.0.0.dylib
  Referenced from: /Users/jenny/resources/R/revdep_library/RGtk2/libs/RGtk2.so
  Reason: image not found
```

## SchemaOnRead (1.0.2)
Maintainer: Michael North <north@anl.gov>

1 error  | 1 warning  | 0 notes

```
checking tests ... ERROR
Running the tests in ‘tests/testthat.R’ failed.
Last 13 lines of output:
  6: suppressWarnings(results <- processor(path, processors, verbose))
  7: withCallingHandlers(expr, warning = function(w) invokeRestart("muffleWarning"))
  8: processor(path, processors, verbose)
  9: readbitmap::read.bitmap(path)
  10: readfun(f, ...)
  
  testthat results ================================================================
  OK: 19 SKIPPED: 0 FAILED: 2
  1. Error: (unknown) (@test.01.R#14) 
  2. Error: (unknown) (@test.04.R#14) 
  
  Error: testthat unit tests failed
  Execution halted

checking re-building of vignette outputs ... WARNING
Error in re-building vignettes:
  ...
Quitting from lines 33-36 (vignettes.Rmd) 
Error: processing vignette 'vignettes.Rmd' failed with diagnostics:
unable to initialize libpng
Execution halted

```

