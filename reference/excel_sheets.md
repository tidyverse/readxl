# List all sheets in an excel spreadsheet

The list of sheet names is especially useful when you want to iterate
over all of the sheets in a workbook. The [readxl Workflows
article](https://readxl.tidyverse.org/articles/readxl-workflows.html)
provides several worked examples of this, showing how to combine readxl
with other packages in the tidyverse, such as purrr, or with base R
functions like [`lapply()`](https://rdrr.io/r/base/lapply.html).

## Usage

``` r
excel_sheets(path)
```

## Arguments

- path:

  Path to the xls/xlsx file.

## Examples

``` r
excel_sheets(readxl_example("datasets.xlsx"))
#> [1] "mtcars"   "chickwts" "quakes"  
excel_sheets(readxl_example("datasets.xls"))
#> [1] "mtcars"   "chickwts" "quakes"  

# To load all sheets in a workbook, use lapply()
path <- readxl_example("datasets.xls")
lapply(excel_sheets(path), read_excel, path = path)
#> [[1]]
#> # A tibble: 32 × 11
#>      mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#>    <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#>  1  21       6  160    110  3.9   2.62  16.5     0     1     4     4
#>  2  21       6  160    110  3.9   2.88  17.0     0     1     4     4
#>  3  22.8     4  108     93  3.85  2.32  18.6     1     1     4     1
#>  4  21.4     6  258    110  3.08  3.22  19.4     1     0     3     1
#>  5  18.7     8  360    175  3.15  3.44  17.0     0     0     3     2
#>  6  18.1     6  225    105  2.76  3.46  20.2     1     0     3     1
#>  7  14.3     8  360    245  3.21  3.57  15.8     0     0     3     4
#>  8  24.4     4  147.    62  3.69  3.19  20       1     0     4     2
#>  9  22.8     4  141.    95  3.92  3.15  22.9     1     0     4     2
#> 10  19.2     6  168.   123  3.92  3.44  18.3     1     0     4     4
#> # ℹ 22 more rows
#> 
#> [[2]]
#> # A tibble: 71 × 2
#>    weight feed     
#>     <dbl> <chr>    
#>  1    179 horsebean
#>  2    160 horsebean
#>  3    136 horsebean
#>  4    227 horsebean
#>  5    217 horsebean
#>  6    168 horsebean
#>  7    108 horsebean
#>  8    124 horsebean
#>  9    143 horsebean
#> 10    140 horsebean
#> # ℹ 61 more rows
#> 
#> [[3]]
#> # A tibble: 1,000 × 5
#>      lat  long depth   mag stations
#>    <dbl> <dbl> <dbl> <dbl>    <dbl>
#>  1 -20.4  182.   562   4.8       41
#>  2 -20.6  181.   650   4.2       15
#>  3 -26    184.    42   5.4       43
#>  4 -18.0  182.   626   4.1       19
#>  5 -20.4  182.   649   4         11
#>  6 -19.7  184.   195   4         12
#>  7 -11.7  166.    82   4.8       43
#>  8 -28.1  182.   194   4.4       15
#>  9 -28.7  182.   211   4.7       35
#> 10 -17.5  180.   622   4.3       19
#> # ℹ 990 more rows
#> 
```
