# Determine file format

Determine if files are xls or xlsx (or from the xlsx family).

`excel_format(guess = TRUE)` is used by
[`read_excel()`](https://readxl.tidyverse.org/dev/reference/read_excel.md)
to determine format. It draws on logic from two lower level functions:

- `format_from_ext()` attempts to determine format from the file
  extension.

- `format_from_signature()` consults the [file
  signature](https://en.wikipedia.org/wiki/List_of_file_signatures) or
  "magic number".

File extensions associated with xlsx vs. xls:

- xlsx: `.xlsx`, `.xlsm`, `.xltx`, `.xltm`

- xls: `.xls`

File signatures (in hexadecimal) for xlsx vs xls:

- xlsx: First 4 bytes are `50 4B 03 04`

- xls: First 8 bytes are `D0 CF 11 E0 A1 B1 1A E1`

## Usage

``` r
excel_format(path, guess = TRUE)

format_from_ext(path)

format_from_signature(path)
```

## Arguments

- path:

  Path to the xls/xlsx file.

- guess:

  Logical. If the file extension is absent or not recognized, this
  controls whether we attempt to guess format based on the file
  signature or "magic number".

## Value

Character vector with values `"xlsx"`, `"xls"`, or `NA`.

## Examples

``` r
files <- c(
  "a.xlsx",
  "b.xls",
  "c.png",
  file.path(R.home("doc"), "html", "logo.jpg"),
  readxl_example("clippy.xlsx"),
  readxl_example("deaths.xls")
)
excel_format(files)
#> [1] "xlsx" "xls"  NA     NA     "xlsx" "xls" 
```
