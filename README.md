# exell

[![Travis-CI Build Status](https://travis-ci.org/hadley/exell.png?branch=master)](https://travis-ci.org/hadley/exell)

The exell package makes it easy to get data out of Excel and into R. Compared to the existing packages (e.g. gdata, xlsx, xlsReadWrite etc) exell has no external dependencies so it's easy to install and use on all operating systems.  It is designed to work with _tabular_ data stored in a single sheet.

Exell supports both the legacy `.xls` format and the modern xml-based `.xlsx` format. `.xls` support is made possible the with [libxls](http://sourceforge.net/projects/libxls/) C library, which abstracts away many of the complexities of the underlying binary format. To parse `.xlsx`, we use the [RapidXML](http://rapidxml.sourceforge.net) C++ library.

## Installation

Exell is not currently available from CRAN, but you can install it from github with:

```R
# install.packages("devtools")
devtools::install_github("hadley/exell")
```

## Usage

```R
library(excell)

read_xls("my-spreadsheet.xls")
read_xls("my-spreadsheet.xls", sheet = "data")
read_xls("my-spreadsheet.xls", na = "NA")
```

## Features

* Re-encodes non-ASCII characters to UTF-8.

* Loads datetimes into POSIXct columns. Both Windows (1900) and Mac (1904) 
  date specifications are processed correctly.

* Blank columns are automatically dropped.

* It returns data frames with additional `tbl_df` class, so if you have
  dplyr loaded, you get nicer printing.
