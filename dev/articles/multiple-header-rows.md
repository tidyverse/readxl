# Multiple Header Rows

``` r

library(magrittr)
library(readxl)
```

A common problem when reading data into R is having multiple header rows
in the source excel file. Take a look at this messy excel file from
sheet 2 in the `clippy.xlsx` example file, which ships with readxl:

``` r

readxl_example("clippy.xlsx") |>
    read_excel(sheet = 2)
#> # A tibble: 2 × 4
#>   name       species              death                 weight    
#>   <chr>      <chr>                <chr>                 <chr>     
#> 1 (at birth) (office supply type) (date is approximate) (in grams)
#> 2 Clippy     paperclip            39083                 0.9
```

**Problem:** The column names are right, but the first row of data is
actually not data- it is additional metadata. This row of metadata is
also causing all the columns to import as character. In reality, `death`
is a datetime and `weight` is numeric.

**Solution:** We can use the
[`read_excel()`](https://readxl.tidyverse.org/dev/reference/read_excel.md)
function to read in the same file twice. In Step 1, we’ll create a
character vector of the column names only. In Step 2, we’ll read in the
actual data and skip the multiple header rows at the top. When we do
this, we lose the column names, so we use the character vector of column
names we created in Step 1 instead.

## Step 1

In this step, we read in the first row only (by setting `n_max = 0`),
extract the names from that row (using the
[`names()`](https://rdrr.io/r/base/names.html) function), and assign
those to a character vector called `cnames`. This object now contains
the correct column names that we’ll need in Step 2.

``` r

(cnames <- readxl_example("clippy.xlsx") |>
    read_excel(sheet = 2, n_max = 0) |>
    names())
#> [1] "name"    "species" "death"   "weight"
```

## Step 2

Now we’ll read in all the rows except for the first two rows (using
`skip = 2`), which contained the variable names and variable
descriptions, and set the column names to `cnames`, which we created in
Step 1. Now our column types are guessed correctly.

``` r

(clippy2 <- readxl_example("clippy.xlsx") |>
  read_excel(sheet = 2, skip = 2, col_names = cnames))
#> # A tibble: 1 × 4
#>   name   species   death               weight
#>   <chr>  <chr>     <dttm>               <dbl>
#> 1 Clippy paperclip 2007-01-01 00:00:00    0.9
```

If you want a way to save that metadata without polluting your actual
data, you can do a third read using `n_max = 1`:

``` r

(clippy_meta <- readxl_example("clippy.xlsx") |>
  read_excel(sheet = 2, n_max = 1))
#> # A tibble: 1 × 4
#>   name       species              death                 weight    
#>   <chr>      <chr>                <chr>                 <chr>     
#> 1 (at birth) (office supply type) (date is approximate) (in grams)
```

> This vignette has been adapted from a post on [Alison Hill’s
> blog](https://www.apreshill.com/blog/2018-07-multiple-headers/).
