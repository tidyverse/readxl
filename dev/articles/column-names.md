# Column Names

``` r
library(readxl)
```

## Column names via `col_names`

readxl has always let you specify `col_names` explicitly at the time of
import:

``` r
read_excel(
  readxl_example("datasets.xlsx"), sheet = "chickwts",
  col_names = c("chick_weight", "chick_ate_this"), skip = 1
)
#> # A tibble: 71 × 2
#>    chick_weight chick_ate_this
#>           <dbl> <chr>         
#>  1          179 horsebean     
#>  2          160 horsebean     
#>  3          136 horsebean     
#>  4          227 horsebean     
#>  5          217 horsebean     
#>  6          168 horsebean     
#>  7          108 horsebean     
#>  8          124 horsebean     
#>  9          143 horsebean     
#> 10          140 horsebean     
#> # ℹ 61 more rows
```

But users have long wanted a way to specify a name repair *strategy*, as
opposed to enumerating the actual column names.

## Built-in levels of `.name_repair`

As of v1.2.0, readxl provides the `.name_repair` argument, which affords
control over how column names are checked or repaired.

The `.name_repair` argument in
[`read_excel()`](https://readxl.tidyverse.org/dev/reference/read_excel.md),
[`read_xls()`](https://readxl.tidyverse.org/dev/reference/read_excel.md),
and
[`read_xlsx()`](https://readxl.tidyverse.org/dev/reference/read_excel.md)
works exactly the same way as it does in
[`tibble::tibble()`](https://tibble.tidyverse.org/reference/tibble.html)
and
[`tibble::as_tibble()`](https://tibble.tidyverse.org/reference/as_tibble.html).
The reasoning behind the name repair strategy is laid out in
[design.tidyverse.org](https://design.tidyverse.org/names.html).

readxl’s default is `.name_repair = "unique"`, which ensures each column
has a unique name. If that is already true of the column names, readxl
won’t touch them.

The value `.name_repair = "universal"` goes further and makes column
names syntactic, i.e. makes sure they don’t contain any forbidden
characters or reserved words. This makes life easier if you use packages
like ggplot2 and dplyr downstream, because the column names will “just
work” everywhere and won’t require protection via backtick quotes.

Compare the column names in these two calls. This shows the difference
between `"unique"` (names can contain spaces) and `"universal"` (spaces
replaced by `.`).

``` r
read_excel(
  readxl_example("deaths.xlsx"),  range = "arts!A5:F8"
)
#> # A tibble: 3 × 6
#>   Name          Profession   Age `Has kids` `Date of birth`    
#>   <chr>         <chr>      <dbl> <lgl>      <dttm>             
#> 1 David Bowie   musician      69 TRUE       1947-01-08 00:00:00
#> 2 Carrie Fisher actor         60 TRUE       1956-10-21 00:00:00
#> 3 Chuck Berry   musician      90 TRUE       1926-10-18 00:00:00
#> # ℹ 1 more variable: `Date of death` <dttm>

read_excel(
  readxl_example("deaths.xlsx"), range = "arts!A5:F8",
  .name_repair = "universal"
)
#> New names:
#> • `Has kids` -> `Has.kids`
#> • `Date of birth` -> `Date.of.birth`
#> • `Date of death` -> `Date.of.death`
#> # A tibble: 3 × 6
#>   Name          Profession   Age Has.kids Date.of.birth      
#>   <chr>         <chr>      <dbl> <lgl>    <dttm>             
#> 1 David Bowie   musician      69 TRUE     1947-01-08 00:00:00
#> 2 Carrie Fisher actor         60 TRUE     1956-10-21 00:00:00
#> 3 Chuck Berry   musician      90 TRUE     1926-10-18 00:00:00
#> # ℹ 1 more variable: Date.of.death <dttm>
```

If you don’t want readxl to touch your column names at all, use
`.name_repair = "minimal"`.

## Pass a function to `.name_repair`

The `.name_repair` argument also accepts a function – pre-existing or
written by you – or an anonymous formula. This function must operate on
a “names in, names out” basis.

``` r
## ALL CAPS! via built-in toupper()
read_excel(readxl_example("clippy.xlsx"), .name_repair = toupper)
#> # A tibble: 4 × 2
#>   NAME                 VALUE    
#>   <chr>                <chr>    
#> 1 Name                 Clippy   
#> 2 Species              paperclip
#> 3 Approx date of death 39083    
#> 4 Weight in grams      0.9

## lower_snake_case via a custom function
my_custom_name_repair <- function(nms) tolower(gsub("[.]", "_", nms))
read_excel(
  readxl_example("datasets.xlsx"), n_max = 3,
  .name_repair = my_custom_name_repair
)
#> # A tibble: 3 × 11
#>     mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#>   <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#> 1  21       6   160   110  3.9   2.62  16.5     0     1     4     4
#> 2  21       6   160   110  3.9   2.88  17.0     0     1     4     4
#> 3  22.8     4   108    93  3.85  2.32  18.6     1     1     4     1

## take first 3 characters via anonymous function
read_excel(
  readxl_example("datasets.xlsx"),
  sheet = "chickwts", n_max = 3,
  .name_repair = ~ substr(.x, start = 1, stop = 3)
)
#> # A tibble: 3 × 2
#>     wei fee      
#>   <dbl> <chr>    
#> 1   179 horsebean
#> 2   160 horsebean
#> 3   136 horsebean
```

This means you can also perform name repair in the style of base R or
another package, such as `janitor::make_clean_names()`.

``` r
read_excel(
  SOME_SPREADSHEET,
  .name_repair = ~ make.names(.x, unique = TRUE)
)

read_excel(
  SOME_SPREADSHEET,
  .name_repair = janitor::make_clean_names
)
```

What if you have a spreadsheet with lots of missing column names? Here’s
how you could fall back to letter-based column names, for easier
troubleshooting.

``` r
read_excel(
  SOME_SPREADSHEET,
  .name_repair = ~ ifelse(nzchar(.x), .x, LETTERS[seq_along(.x)])
) 
```
