# readxl

[![Travis-CI Build Status](https://travis-ci.org/hadley/readxl.png?branch=master)](https://travis-ci.org/hadley/readxl)
[![CRAN_Status_Badge](http://www.r-pkg.org/badges/version/readxl)](http://cran.r-project.org/package=readxl)

The readxl package makes it easy to get data out of Excel and into R. Compared to many of the existing packages (e.g. gdata, xlsx, xlsReadWrite) readxl has no external dependencies so it's easy to install and use on all operating systems.  It is designed to work with _tabular_ data stored in a single sheet.

Readxl supports both the legacy `.xls` format and the modern xml-based `.xlsx` format. `.xls` support is made possible the with [libxls](http://sourceforge.net/projects/libxls/) C library, which abstracts away many of the complexities of the underlying binary format. To parse `.xlsx`, we use the [RapidXML](http://rapidxml.sourceforge.net) C++ library.

## Installation

You can install:

* the latest released version from CRAN with:
*
```R
install.packages("readxl")
```

* the latest development version from github with:

```R
# install.packages("devtools")
devtools::install_github("hadley/readxl")
```

## Usage

```R
library(readxl)

# read_excel reads both xls and xlsx files
read_excel("my-old-spreadsheet.xls")
read_excel("my-new-spreadsheet.xlsx")

# Specify sheet with a number or name
read_excel("my-spreadsheet.xls", sheet = "data")
read_excel("my-spreadsheet.xls", sheet = 2)

# If NAs are represented by something other than blank cells,
# set the na argument
read_excel("my-spreadsheet.xls", na = "NA")
```

## Features

* Re-encodes non-ASCII characters to UTF-8.

* Loads datetimes into POSIXct columns. Both Windows (1900) and Mac (1904) 
  date specifications are processed correctly.

* Blank columns and rows are automatically dropped.

* It returns data frames with additional `tbl_df` class, so if you have
  dplyr loaded, you get nicer printing.

## Example data

The package includes an example file created with openxlsx:

```R
l <- list("iris" = iris, "mtcars" = mtcars, chickwts = chickwts, quakes = quakes)
openxlsx::write.xlsx(l, file = "inst/extdata/datasets.xlsx")
```
