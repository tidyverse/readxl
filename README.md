# exell

[![Travis-CI Build Status](https://travis-ci.org/hadley/exell.png?branch=master)](https://travis-ci.org/hadley/exell)

The exell package makes it easy to get data out of Excel and into R. Compared to the existing packages (e.g. gdata, xlsx, xlsReadWrite etc) exell has no external dependencies so it's easy to install and use on all operating systems.  It is designed to work with _tabular_ data stored in a single sheet.

It currently supports only the legacy `.xls` format, but `.xlsx` support is planned.

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
