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

37 packages

|package           |version | errors| warnings| notes|
|:-----------------|:-------|------:|--------:|-----:|
|BEACH             |1.1.2   |      0|        0|     0|
|chillR            |0.66    |      0|        0|     0|
|ckanr             |0.1.0   |      0|        0|     0|
|CONS              |0.1.1   |      0|        0|     0|
|DataLoader        |1.3     |      1|        0|     0|
|DCM               |0.1.1   |      0|        0|     0|
|DLMtool           |3.2.3   |      0|        0|     0|
|dpcR              |0.4     |      0|        0|     0|
|elementR          |1.3.0   |      1|        0|     0|
|evaluator         |0.1.0   |      0|        0|     0|
|ezec              |1.0.1   |      0|        0|     0|
|GerminaR          |1.1     |      0|        0|     0|
|GetTDData         |1.2.5   |      0|        0|     0|
|heemod            |0.9.0   |      1|        0|     0|
|joinXL            |1.0.1   |      0|        0|     0|
|lessR             |3.6.0   |      0|        0|     0|
|Luminescence      |0.7.4   |      0|        0|     0|
|manifestoR        |1.2.3   |      0|        0|     0|
|photobiologyInOut |0.4.13  |      0|        0|     0|
|popprxl           |0.1.3   |      0|        0|     0|
|QuantTools        |0.5.4   |      0|        0|     1|
|rattle            |4.1.0   |      1|        5|     4|
|raw               |0.1.4   |      0|        0|     0|
|Rcmdr             |2.3-2   |      0|        0|     1|
|RcmdrMisc         |1.0-5   |      0|        0|     0|
|RcmdrPlugin.EZR   |1.35    |      0|        0|     0|
|RDML              |0.9-5   |      0|        0|     0|
|rio               |0.4.16  |      0|        0|     0|
|rotl              |3.0.3   |      0|        0|     0|
|SchemaOnRead      |1.0.2   |      1|        1|     0|
|shinyHeatmaply    |0.1.0   |      0|        0|     0|
|textreadr         |0.3.1   |      0|        0|     0|
|tidyverse         |1.1.1   |      0|        0|     0|
|tidyxl            |0.2.1   |      0|        0|     0|
|TR8               |0.9.16  |      0|        0|     0|
|unvotes           |0.1.0   |      0|        0|     0|
|xlutils3          |0.1.0   |      0|        0|     0|

## BEACH (1.1.2)
Maintainer: Danni Yu <danni.yu@gmail.com>

0 errors | 0 warnings | 0 notes

## chillR (0.66)
Maintainer: Eike Luedeling <eike@eikeluedeling.com>

0 errors | 0 warnings | 0 notes

## ckanr (0.1.0)
Maintainer: Scott Chamberlain <myrmecocystus@gmail.com>  
Bug reports: http://www.github.com/ropensci/ckanr/issues

0 errors | 0 warnings | 0 notes

## CONS (0.1.1)
Maintainer: Nery Sofia Huerta-Pacheco <nehuerta@uv.mx>

0 errors | 0 warnings | 0 notes

## DataLoader (1.3)
Maintainer: Srivenkatesh Gandhi <srivenkateshg@sase.ssn.edu.in>

1 error  | 0 warnings | 0 notes

```
checking whether package ‘DataLoader’ can be installed ... ERROR
Installation failed.
See ‘/Users/jenny/rrr/readxl/revdep/checks/DataLoader.Rcheck/00install.out’ for details.
```

## DCM (0.1.1)
Maintainer: Nery Sofia Huerta-Pacheco <nehuerta@uv.mx>

0 errors | 0 warnings | 0 notes

## DLMtool (3.2.3)
Maintainer: Tom Carruthers <t.carruthers@fisheries.ubc.ca>  
Bug reports: https://github.com/DLMtool/DLMtool/issues

0 errors | 0 warnings | 0 notes

## dpcR (0.4)
Maintainer: Michal Burdukiewicz <michalburdukiewicz@gmail.com>  
Bug reports: https://github.com/michbur/dpcR/issues

0 errors | 0 warnings | 0 notes

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

## evaluator (0.1.0)
Maintainer: David Severski <davidski@deadheaven.com>  
Bug reports: https://github.com/davidski/evaluator/issues

0 errors | 0 warnings | 0 notes

## ezec (1.0.1)
Maintainer: Zhian N. Kamvar <zkamvar@gmail.com>  
Bug reports: https://github.com/grunwaldlab/ezec/issues

0 errors | 0 warnings | 0 notes

## GerminaR (1.1)
Maintainer: Flavio Lozano Isla <flavjack@gmail.com>  
Bug reports: https://github.com/Flavjack/GerminaR/issues

0 errors | 0 warnings | 0 notes

## GetTDData (1.2.5)
Maintainer: Marcelo Perlin <marceloperlin@gmail.com>

0 errors | 0 warnings | 0 notes

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

## joinXL (1.0.1)
Maintainer: Yvonne Glanville <yvonneglanville@gmail.com>  
Bug reports: http://github.com/yvonneglanville/joinXL/issues

0 errors | 0 warnings | 0 notes

## lessR (3.6.0)
Maintainer: David W. Gerbing <gerbing@pdx.edu>

0 errors | 0 warnings | 0 notes

## Luminescence (0.7.4)
Maintainer: Sebastian Kreutzer <sebastian.kreutzer@u-bordeaux-montaigne.fr>  
Bug reports: https://github.com/R-Lum/Luminescence/issues

0 errors | 0 warnings | 0 notes

## manifestoR (1.2.3)
Maintainer: Jirka Lewandowski <jirka.lewandowski@wzb.eu>  
Bug reports: https://github.com/ManifestoProject/manifestoR/issues

0 errors | 0 warnings | 0 notes

## photobiologyInOut (0.4.13)
Maintainer: Pedro J. Aphalo <pedro.aphalo@helsinki.fi>  
Bug reports: https://bitbucket.org/aphalo/photobiologyinout/issues/

0 errors | 0 warnings | 0 notes

## popprxl (0.1.3)
Maintainer: Zhian N. Kamvar <zkamvar@gmail.com>  
Bug reports: https://github.com/zkamvar/popprxl/issues

0 errors | 0 warnings | 0 notes

## QuantTools (0.5.4)
Maintainer: Stanislav Kovalevsky <so.kovalevsky@gmail.com>  
Bug reports: https://bitbucket.org/quanttools/quanttools/issues

0 errors | 0 warnings | 1 note 

```
checking installed package size ... NOTE
  installed size is  5.3Mb
  sub-directories of 1Mb or more:
    data   4.5Mb
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

## raw (0.1.4)
Maintainer: Brian A. Fannin <FanninQED@yahoo.com>

0 errors | 0 warnings | 0 notes

## Rcmdr (2.3-2)
Maintainer: John Fox <jfox@mcmaster.ca>

0 errors | 0 warnings | 1 note 

```
checking installed package size ... NOTE
  installed size is  7.5Mb
  sub-directories of 1Mb or more:
    doc   4.0Mb
    po    2.0Mb
```

## RcmdrMisc (1.0-5)
Maintainer: John Fox <jfox@mcmaster.ca>

0 errors | 0 warnings | 0 notes

## RcmdrPlugin.EZR (1.35)
Maintainer: Yoshinobu Kanda <ycanda-tky@umin.ac.jp>

0 errors | 0 warnings | 0 notes

## RDML (0.9-5)
Maintainer: Konstantin A. Blagodatskikh <k.blag@yandex.ru>

0 errors | 0 warnings | 0 notes

## rio (0.4.16)
Maintainer: Thomas J. Leeper <thosjleeper@gmail.com>  
Bug reports: https://github.com/leeper/rio/issues

0 errors | 0 warnings | 0 notes

## rotl (3.0.3)
Maintainer: Francois Michonneau <francois.michonneau@gmail.com>  
Bug reports: https://github.com/ropensci/rotl/issues

0 errors | 0 warnings | 0 notes

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

## shinyHeatmaply (0.1.0)
Maintainer: Jonathan Sidi <yonicd@gmail.com>  
Bug reports: https://github.com/yonicd/shinyHeatmaply/issues

0 errors | 0 warnings | 0 notes

## textreadr (0.3.1)
Maintainer: Tyler Rinker <tyler.rinker@gmail.com>  
Bug reports: https://github.com/trinker/textreadr/issues?state=open

0 errors | 0 warnings | 0 notes

## tidyverse (1.1.1)
Maintainer: Hadley Wickham <hadley@rstudio.com>  
Bug reports: https://github.com/tidyverse/tidyverse/issues

0 errors | 0 warnings | 0 notes

## tidyxl (0.2.1)
Maintainer: Duncan Garmonsway <nacnudus@gmail.com>  
Bug reports: https://github.com/nacnudus/tidyxl/issues

0 errors | 0 warnings | 0 notes

## TR8 (0.9.16)
Maintainer: Gionata Bocci <boccigionata@gmail.com>  
Bug reports: https://github.com/GioBo/TR8/issues

0 errors | 0 warnings | 0 notes

## unvotes (0.1.0)
Maintainer: David Robinson <admiral.david@gmail.com>  
Bug reports: http://github.com/dgrtwo/unvotes/issues

0 errors | 0 warnings | 0 notes

## xlutils3 (0.1.0)
Maintainer: Aurele Morvan <aurele@saagie.com>

0 errors | 0 warnings | 0 notes

