# Specify cells for reading

The `range` argument of
[`read_excel()`](https://readxl.tidyverse.org/dev/reference/read_excel.md)
provides many ways to limit the read to a specific rectangle of cells.
The simplest usage is to provide an Excel-like cell range, such as
`range = "D12:F15"` or `range ="R1C12:R6C15"`. The cell rectangle can be
specified in various other ways, using helper functions. You can find
more examples at the [sheet
geometry](https://readxl.tidyverse.org/articles/sheet-geometry.html#range)
vignette. In all cases, cell range processing is handled by the
[cellranger](https://rdrr.io/pkg/cellranger/man/cellranger.html)
package, where you can find full documentation for the functions used in
the examples below.

## See also

The [cellranger](https://rdrr.io/pkg/cellranger/man/cellranger.html)
package has full documentation on cell specification and offers
additional functions for manipulating "A1:D10" style spreadsheet ranges.
Here are the most relevant:

- [`cellranger::cell_limits()`](https://rdrr.io/pkg/cellranger/man/cell_limits.html)

- [`cellranger::cell_rows()`](https://rdrr.io/pkg/cellranger/man/cell_rows.html)

- [`cellranger::cell_cols()`](https://rdrr.io/pkg/cellranger/man/cell_cols.html)

- [`cellranger::anchored()`](https://rdrr.io/pkg/cellranger/man/anchored.html)

## Examples

``` r
path <- readxl_example("geometry.xls")
## Rows 1 and 2 are empty (as are rows 7 and higher)
## Column 1 aka "A" is empty (as are columns 5 of "E" and higher)

# By default, the populated data cells are "shrink-wrapped" into a
# minimal data frame
read_excel(path)
#> # A tibble: 3 × 3
#>   B3    C3    D3   
#>   <chr> <chr> <chr>
#> 1 B4    C4    D4   
#> 2 B5    C5    D5   
#> 3 B6    C6    D6   

# Specific rectangle that is subset of populated cells, possibly improper
read_excel(path, range = "B3:D6")
#> # A tibble: 3 × 3
#>   B3    C3    D3   
#>   <chr> <chr> <chr>
#> 1 B4    C4    D4   
#> 2 B5    C5    D5   
#> 3 B6    C6    D6   
read_excel(path, range = "C3:D5")
#> # A tibble: 2 × 2
#>   C3    D3   
#>   <chr> <chr>
#> 1 C4    D4   
#> 2 C5    D5   

# Specific rectangle that forces inclusion of unpopulated cells
read_excel(path, range = "A3:D5")
#> New names:
#> • `` -> `...1`
#> # A tibble: 2 × 4
#>   ...1  B3    C3    D3   
#>   <lgl> <chr> <chr> <chr>
#> 1 NA    B4    C4    D4   
#> 2 NA    B5    C5    D5   
read_excel(path, range = "A4:E5")
#> New names:
#> • `` -> `...1`
#> • `` -> `...5`
#> # A tibble: 1 × 5
#>   ...1  B4    C4    D4    ...5 
#>   <lgl> <chr> <chr> <chr> <lgl>
#> 1 NA    B5    C5    D5    NA   
read_excel(path, range = "C5:E7")
#> New names:
#> • `` -> `...3`
#> # A tibble: 2 × 3
#>   C5    D5    ...3 
#>   <chr> <chr> <lgl>
#> 1 C6    D6    NA   
#> 2 NA    NA    NA   

# Anchor a rectangle of specified size at a particular cell
read_excel(path, range = anchored("C4", dim = c(3, 2)), col_names = FALSE)
#> New names:
#> • `` -> `...1`
#> • `` -> `...2`
#> # A tibble: 3 × 2
#>   ...1  ...2 
#>   <chr> <chr>
#> 1 C4    D4   
#> 2 C5    D5   
#> 3 C6    D6   

# Specify only the rows
read_excel(path, range = cell_rows(3:5))
#> # A tibble: 2 × 3
#>   B3    C3    D3   
#>   <chr> <chr> <chr>
#> 1 B4    C4    D4   
#> 2 B5    C5    D5   
## is equivalent to
read_excel(path, range = cell_rows(c(3, 5)))
#> # A tibble: 2 × 3
#>   B3    C3    D3   
#>   <chr> <chr> <chr>
#> 1 B4    C4    D4   
#> 2 B5    C5    D5   

# Specify only the columns by column number or letter
read_excel(path, range = cell_cols("C:D"))
#> # A tibble: 3 × 2
#>   C3    D3   
#>   <chr> <chr>
#> 1 C4    D4   
#> 2 C5    D5   
#> 3 C6    D6   
read_excel(path, range = cell_cols(2))
#> # A tibble: 3 × 1
#>   B3   
#>   <chr>
#> 1 B4   
#> 2 B5   
#> 3 B6   

# Specify exactly one row or column bound
read_excel(path, range = cell_rows(c(5, NA)))
#> # A tibble: 1 × 3
#>   B5    C5    D5   
#>   <chr> <chr> <chr>
#> 1 B6    C6    D6   
read_excel(path, range = cell_rows(c(NA, 4)))
#> New names:
#> • `` -> `...1`
#> • `` -> `...2`
#> • `` -> `...3`
#> # A tibble: 3 × 3
#>   ...1  ...2  ...3 
#>   <chr> <chr> <chr>
#> 1 NA    NA    NA   
#> 2 B3    C3    D3   
#> 3 B4    C4    D4   
read_excel(path, range = cell_cols(c("C", NA)))
#> # A tibble: 3 × 2
#>   C3    D3   
#>   <chr> <chr>
#> 1 C4    D4   
#> 2 C5    D5   
#> 3 C6    D6   
read_excel(path, range = cell_cols(c(NA, 2)))
#> New names:
#> • `` -> `...1`
#> # A tibble: 3 × 2
#>   ...1  B3   
#>   <lgl> <chr>
#> 1 NA    B4   
#> 2 NA    B5   
#> 3 NA    B6   

# General open rectangles
# upper left = C4, everything else unspecified
read_excel(path, range = cell_limits(c(4, 3), c(NA, NA)))
#> # A tibble: 2 × 2
#>   C4    D4   
#>   <chr> <chr>
#> 1 C5    D5   
#> 2 C6    D6   
# upper right = D4, everything else unspecified
read_excel(path, range = cell_limits(c(4, NA), c(NA, 4)))
#> New names:
#> • `` -> `...1`
#> # A tibble: 2 × 4
#>   ...1  B4    C4    D4   
#>   <lgl> <chr> <chr> <chr>
#> 1 NA    B5    C5    D5   
#> 2 NA    B6    C6    D6   
```
