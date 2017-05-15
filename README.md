
<!-- README.md is generated from README.Rmd. Please edit that file -->
readxl <img src="tools/logo.png" align="right" />
=================================================

[![Travis-CI Build Status](https://travis-ci.org/tidyverse/readxl.svg?branch=master)](https://travis-ci.org/tidyverse/readxl) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/tidyverse/readxl?branch=master&svg=true)](https://ci.appveyor.com/project/tidyverse/readxl) [![Coverage Status](https://img.shields.io/codecov/c/github/tidyverse/readxl/master.svg)](https://codecov.io/github/tidyverse/readxl?branch=master) [![CRAN\_Status\_Badge](https://www.r-pkg.org/badges/version/readxl)](https://cran.r-project.org/package=readxl)

Overview
--------

The readxl package makes it easy to get data out of Excel and into R. Compared to many of the existing packages (e.g. gdata, xlsx, xlsReadWrite) readxl has no external dependencies, so it's easy to install and use on all operating systems. It is designed to work with *tabular* data.

readxl supports both the legacy `.xls` format and the modern xml-based `.xlsx` format. The [libxls](https://sourceforge.net/projects/libxls/) C library is used to support `.xls`, which abstracts away many of the complexities of the underlying binary format. To parse `.xlsx`, we use the [RapidXML](http://rapidxml.sourceforge.net) C++ library.

Installation
------------

The easiest way to install the latest released version from CRAN is to install the whole tidyverse.

``` r
install.packages("tidyverse")
```

NOTE: you will still need to load readxl explicitly, because it is not a core tidyverse package loaded via `library(tidyverse)`.

Alternatively, install just readxl from CRAN:

``` r
install.packages("readxl")
```

Or install the development version from GitHub:

``` r
# install.packages("devtools")
devtools::install_github("tidyverse/readxl")
```

Usage
-----

``` r
library(readxl)
```

readxl includes several example files, which we use throughout the documentation. Use the helper `readxl_example()` with no arguments to list them or call it with an example filename to get the path.

``` r
readxl_example()
#>  [1] "clippy.xls"    "clippy.xlsx"   "datasets.xls"  "datasets.xlsx"
#>  [5] "deaths.xls"    "deaths.xlsx"   "geometry.xls"  "geometry.xlsx"
#>  [9] "type-me.xls"   "type-me.xlsx"
readxl_example("clippy.xls")
#> [1] "/Users/jenny/resources/R/library/readxl/extdata/clippy.xls"
```

`read_excel()` reads both xls and xlsx files and detects the format from the extension.

``` r
xlsx_example <- readxl_example("datasets.xlsx")
read_excel(xlsx_example)
#> # A tibble: 150 x 5
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#>          <dbl>       <dbl>        <dbl>       <dbl>   <chr>
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> # ... with 147 more rows

xls_example <- readxl_example("datasets.xls")
read_excel(xls_example)
#> # A tibble: 150 x 5
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#>          <dbl>       <dbl>        <dbl>       <dbl>   <chr>
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
#> # ... with 147 more rows
```

List the sheet names with `excel_sheets()`.

``` r
excel_sheets(xlsx_example)
#> [1] "iris"     "mtcars"   "chickwts" "quakes"
```

Specify a worksheet by name or number.

``` r
read_excel(xlsx_example, sheet = "chickwts")
#> # A tibble: 71 x 2
#>   weight      feed
#>    <dbl>     <chr>
#> 1    179 horsebean
#> 2    160 horsebean
#> 3    136 horsebean
#> # ... with 68 more rows
read_excel(xls_example, sheet = 4)
#> # A tibble: 1,000 x 5
#>      lat   long depth   mag stations
#>    <dbl>  <dbl> <dbl> <dbl>    <dbl>
#> 1 -20.42 181.62   562   4.8       41
#> 2 -20.62 181.03   650   4.2       15
#> 3 -26.00 184.10    42   5.4       43
#> # ... with 997 more rows
```

There are various ways to control which cells are read. You can even specify the sheet here, if providing an Excel-style cell range.

``` r
read_excel(xlsx_example, n_max = 3)
#> # A tibble: 3 x 5
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#>          <dbl>       <dbl>        <dbl>       <dbl>   <chr>
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
read_excel(xlsx_example, range = "C1:E4")
#> # A tibble: 3 x 3
#>   Petal.Length Petal.Width Species
#>          <dbl>       <dbl>   <chr>
#> 1          1.4         0.2  setosa
#> 2          1.4         0.2  setosa
#> 3          1.3         0.2  setosa
read_excel(xlsx_example, range = cell_rows(1:4))
#> # A tibble: 3 x 5
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#>          <dbl>       <dbl>        <dbl>       <dbl>   <chr>
#> 1          5.1         3.5          1.4         0.2  setosa
#> 2          4.9         3.0          1.4         0.2  setosa
#> 3          4.7         3.2          1.3         0.2  setosa
read_excel(xlsx_example, range = cell_cols("B:D"))
#> # A tibble: 150 x 3
#>   Sepal.Width Petal.Length Petal.Width
#>         <dbl>        <dbl>       <dbl>
#> 1         3.5          1.4         0.2
#> 2         3.0          1.4         0.2
#> 3         3.2          1.3         0.2
#> # ... with 147 more rows
read_excel(xlsx_example, range = "mtcars!B1:D5")
#> # A tibble: 4 x 3
#>     cyl  disp    hp
#>   <dbl> <dbl> <dbl>
#> 1     6   160   110
#> 2     6   160   110
#> 3     4   108    93
#> # ... with 1 more rows
```

If `NA`s are represented by something other than blank cells, set the `na` argument.

``` r
read_excel(xlsx_example, na = "setosa")
#> # A tibble: 150 x 5
#>   Sepal.Length Sepal.Width Petal.Length Petal.Width Species
#>          <dbl>       <dbl>        <dbl>       <dbl>   <chr>
#> 1          5.1         3.5          1.4         0.2    <NA>
#> 2          4.9         3.0          1.4         0.2    <NA>
#> 3          4.7         3.2          1.3         0.2    <NA>
#> # ... with 147 more rows
```

If you are new to the tidyverse conventions for data import, you may want to consult the [data import chapter](http://r4ds.had.co.nz/data-import.html) in R for Data Science. readxl will become increasingly consistent with other packages, such as [readr](http://readr.tidyverse.org).

Articles
--------

You can find more detail in [these articles](http://readxl.tidyverse.org/articles/index.html):

-   Cell and Column Types
-   Sheet Geometry
-   readxl Workflows

Features
--------

-   No external dependency on, e.g., Java or Perl.

-   Re-encodes non-ASCII characters to UTF-8.

-   Loads datetimes into POSIXct columns. Both Windows (1900) and Mac (1904) date specifications are processed correctly.

-   Discovers the minimal data rectangle and returns that, by default. User can exert more control with `range`, `skip`, and `n_max`.

-   Column names and types are determined from the data in the sheet, by default. User can also supply via `col_names` and `col_types`.

-   Returns a [tibble](http://tibble.tidyverse.org/reference/tibble.html), i.e. a data frame with an additional `tbl_df` class. Among other things, this provide nicer printing.

Other relevant packages
-----------------------

Here are some other packages with functionality that is complementary to readxl and that also avoid a Java dependency.

**Writing Excel files**: The example files `datasets.xlsx` and `datasets.xls` were created with the help of [openxlsx](https://CRAN.R-project.org/package=openxlsx) (and Excel). openxlsx provides "a high level interface to writing, styling and editing worksheets".

``` r
l <- list(iris = iris, mtcars = mtcars, chickwts = chickwts, quakes = quakes)
openxlsx::write.xlsx(l, file = "inst/extdata/datasets.xlsx")
```

**Non-tabular data and formatting**: [tidyxl](https://cran.r-project.org/package=tidyxl) is focused on importing awkward and non-tabular data from Excel. It also "exposes cell content, position and formatting in a tidy structure for further manipulation".
