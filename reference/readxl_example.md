# Get path to readxl example

readxl comes bundled with some example files in its `inst/extdata`
directory. This function make them easy to access.

## Usage

``` r
readxl_example(path = NULL)
```

## Arguments

- path:

  Name of file. If `NULL`, the example files will be listed.

## Examples

``` r
readxl_example()
#>  [1] "clippy.xls"    "clippy.xlsx"   "datasets.xls"  "datasets.xlsx"
#>  [5] "deaths.xls"    "deaths.xlsx"   "geometry.xls"  "geometry.xlsx"
#>  [9] "type-me.xls"   "type-me.xlsx" 
readxl_example("datasets.xlsx")
#> [1] "/home/runner/work/_temp/Library/readxl/extdata/datasets.xlsx"
```
