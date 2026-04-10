# Read xls and xlsx files

Read xls and xlsx files

`read_excel()` calls
[`excel_format()`](https://readxl.tidyverse.org/dev/reference/excel_format.md)
to determine if `path` is xls or xlsx, based on the file extension and
the file itself, in that order. Use `read_xls()` and `read_xlsx()`
directly if you know better and want to prevent such guessing.

## Usage

``` r
read_excel(
  path,
  sheet = NULL,
  range = NULL,
  col_names = TRUE,
  col_types = NULL,
  na = "",
  trim_ws = TRUE,
  skip = 0,
  n_max = Inf,
  guess_max = min(1000, n_max),
  progress = readxl_progress(),
  .name_repair = "unique"
)

read_xls(
  path,
  sheet = NULL,
  range = NULL,
  col_names = TRUE,
  col_types = NULL,
  na = "",
  trim_ws = TRUE,
  skip = 0,
  n_max = Inf,
  guess_max = min(1000, n_max),
  progress = readxl_progress(),
  .name_repair = "unique"
)

read_xlsx(
  path,
  sheet = NULL,
  range = NULL,
  col_names = TRUE,
  col_types = NULL,
  na = "",
  trim_ws = TRUE,
  skip = 0,
  n_max = Inf,
  guess_max = min(1000, n_max),
  progress = readxl_progress(),
  .name_repair = "unique"
)
```

## Arguments

- path:

  Path to the xls/xlsx file.

- sheet:

  Sheet to read. Either a string (the name of a sheet), or an integer
  (the position of the sheet). Ignored if the sheet is specified via
  `range`. If neither argument specifies the sheet, defaults to the
  first sheet.

- range:

  A cell range to read from, as described in
  [cell-specification](https://readxl.tidyverse.org/dev/reference/cell-specification.md).
  Includes typical Excel ranges like "B3:D87", possibly including the
  sheet name like "Budget!B2:G14", and more. Interpreted strictly, even
  if the range forces the inclusion of leading or trailing empty rows or
  columns. Takes precedence over `skip`, `n_max` and `sheet`.

- col_names:

  `TRUE` to use the first row as column names, `FALSE` to get default
  names, or a character vector giving a name for each column. If user
  provides `col_types` as a vector, `col_names` can have one entry per
  column, i.e. have the same length as `col_types`, or one entry per
  unskipped column.

- col_types:

  Either `NULL` to guess all from the spreadsheet or a character vector
  containing one entry per column from these options: "skip", "guess",
  "logical", "numeric", "date", "text" or "list". If exactly one
  `col_type` is specified, it will be recycled. The content of a cell in
  a skipped column is never read and that column will not appear in the
  data frame output. A list cell loads a column as a list of length 1
  vectors, which are typed using the type guessing logic from
  `col_types = NULL`, but on a cell-by-cell basis.

- na:

  Character vector of strings to interpret as missing values. By
  default, readxl treats blank cells as missing data.

- trim_ws:

  Should leading and trailing whitespace be trimmed?

- skip:

  Minimum number of rows to skip before reading anything, be it column
  names or data. Leading empty rows are automatically skipped, so this
  is a lower bound. Ignored if `range` is given.

- n_max:

  Maximum number of data rows to read. Trailing empty rows are
  automatically skipped, so this is an upper bound on the number of rows
  in the returned tibble. Ignored if `range` is given.

- guess_max:

  Maximum number of data rows to use for guessing column types.

- progress:

  Display a progress spinner? By default, the spinner appears only in an
  interactive session, outside the context of knitting a document, and
  when the call is likely to run for several seconds or more. See
  [`readxl_progress()`](https://readxl.tidyverse.org/dev/reference/readxl_progress.md)
  for more details.

- .name_repair:

  Handling of column names. Passed along to
  [`tibble::as_tibble()`](https://tibble.tidyverse.org/reference/as_tibble.html).
  readxl's default is \`.name_repair = "unique", which ensures column
  names are not empty and are unique.

## Value

A [tibble](https://tibble.tidyverse.org/reference/tibble-package.html)

## See also

[cell-specification](https://readxl.tidyverse.org/dev/reference/cell-specification.md)
for more details on targetting cells with the `range` argument

## Examples

``` r
datasets <- readxl_example("datasets.xlsx")
read_excel(datasets)
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

# Specify sheet either by position or by name
read_excel(datasets, 2)
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
read_excel(datasets, "mtcars")
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

# Skip rows and use default column names
read_excel(datasets, skip = 10, col_names = FALSE)
#> New names:
#> • `` -> `...1`
#> • `` -> `...2`
#> • `` -> `...3`
#> • `` -> `...4`
#> • `` -> `...5`
#> • `` -> `...6`
#> • `` -> `...7`
#> • `` -> `...8`
#> • `` -> `...9`
#> • `` -> `...10`
#> • `` -> `...11`
#> # A tibble: 23 × 11
#>     ...1  ...2  ...3  ...4  ...5  ...6  ...7  ...8  ...9 ...10 ...11
#>    <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#>  1  19.2     6 168.    123  3.92  3.44  18.3     1     0     4     4
#>  2  17.8     6 168.    123  3.92  3.44  18.9     1     0     4     4
#>  3  16.4     8 276.    180  3.07  4.07  17.4     0     0     3     3
#>  4  17.3     8 276.    180  3.07  3.73  17.6     0     0     3     3
#>  5  15.2     8 276.    180  3.07  3.78  18       0     0     3     3
#>  6  10.4     8 472     205  2.93  5.25  18.0     0     0     3     4
#>  7  10.4     8 460     215  3     5.42  17.8     0     0     3     4
#>  8  14.7     8 440     230  3.23  5.34  17.4     0     0     3     4
#>  9  32.4     4  78.7    66  4.08  2.2   19.5     1     1     4     1
#> 10  30.4     4  75.7    52  4.93  1.62  18.5     1     1     4     2
#> # ℹ 13 more rows

# Recycle a single column type
read_excel(datasets, col_types = "text")
#> # A tibble: 32 × 11
#>    mpg      cyl   disp  hp    drat  wt    qsec  vs    am    gear  carb 
#>    <chr>    <chr> <chr> <chr> <chr> <chr> <chr> <chr> <chr> <chr> <chr>
#>  1 21       6     160   110   3.9   2.62  16.46 0     1     4     4    
#>  2 21       6     160   110   3.9   2.875 17.02 0     1     4     4    
#>  3 22.8     4     108   93    3.85  2.31… 18.61 1     1     4     1    
#>  4 21.4     6     258   110   3.08  3.21… 19.4… 1     0     3     1    
#>  5 18.7     8     360   175   3.15  3.44  17.02 0     0     3     2    
#>  6 18.1000… 6     225   105   2.76  3.46  20.22 1     0     3     1    
#>  7 14.3     8     360   245   3.21  3.57  15.84 0     0     3     4    
#>  8 24.4     4     146.… 62    3.69  3.19  20    1     0     4     2    
#>  9 22.8     4     140.… 95    3.92  3.15  22.9  1     0     4     2    
#> 10 19.2     6     167.6 123   3.92  3.44  18.3  1     0     4     4    
#> # ℹ 22 more rows

# Specify some col_types and guess others
read_excel(
  readxl_example("deaths.xlsx"),
  skip = 4, n_max = 10, col_names = TRUE,
  col_types = c("text", "text", "guess", "guess", "guess", "guess")
)
#> # A tibble: 10 × 6
#>    Name               Profession   Age `Has kids` `Date of birth`    
#>    <chr>              <chr>      <dbl> <lgl>      <dttm>             
#>  1 David Bowie        musician      69 TRUE       1947-01-08 00:00:00
#>  2 Carrie Fisher      actor         60 TRUE       1956-10-21 00:00:00
#>  3 Chuck Berry        musician      90 TRUE       1926-10-18 00:00:00
#>  4 Bill Paxton        actor         61 TRUE       1955-05-17 00:00:00
#>  5 Prince             musician      57 TRUE       1958-06-07 00:00:00
#>  6 Alan Rickman       actor         69 FALSE      1946-02-21 00:00:00
#>  7 Florence Henderson actor         82 TRUE       1934-02-14 00:00:00
#>  8 Harper Lee         author        89 FALSE      1926-04-28 00:00:00
#>  9 Zsa Zsa Gábor      actor         99 TRUE       1917-02-06 00:00:00
#> 10 George Michael     musician      53 FALSE      1963-06-25 00:00:00
#> # ℹ 1 more variable: `Date of death` <dttm>

# Accomodate a column with disparate types via col_type = "list"
df <- read_excel(readxl_example("clippy.xlsx"), col_types = c("text", "list"))
df
#> # A tibble: 4 × 2
#>   name                 value     
#>   <chr>                <list>    
#> 1 Name                 <chr [1]> 
#> 2 Species              <chr [1]> 
#> 3 Approx date of death <dttm [1]>
#> 4 Weight in grams      <dbl [1]> 
df$value
#> [[1]]
#> [1] "Clippy"
#> 
#> [[2]]
#> [1] "paperclip"
#> 
#> [[3]]
#> [1] "2007-01-01 UTC"
#> 
#> [[4]]
#> [1] 0.9
#> 
sapply(df$value, class)
#> [[1]]
#> [1] "character"
#> 
#> [[2]]
#> [1] "character"
#> 
#> [[3]]
#> [1] "POSIXct" "POSIXt" 
#> 
#> [[4]]
#> [1] "numeric"
#> 

# Limit the number of data rows read
read_excel(datasets, n_max = 3)
#> # A tibble: 3 × 11
#>     mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#>   <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#> 1  21       6   160   110  3.9   2.62  16.5     0     1     4     4
#> 2  21       6   160   110  3.9   2.88  17.0     0     1     4     4
#> 3  22.8     4   108    93  3.85  2.32  18.6     1     1     4     1

# Read from an Excel range using A1 or R1C1 notation
read_excel(datasets, range = "C1:E7")
#> # A tibble: 6 × 3
#>    disp    hp  drat
#>   <dbl> <dbl> <dbl>
#> 1   160   110  3.9 
#> 2   160   110  3.9 
#> 3   108    93  3.85
#> 4   258   110  3.08
#> 5   360   175  3.15
#> 6   225   105  2.76
read_excel(datasets, range = "R1C2:R2C5")
#> # A tibble: 1 × 4
#>     cyl  disp    hp  drat
#>   <dbl> <dbl> <dbl> <dbl>
#> 1     6   160   110   3.9

# Specify the sheet as part of the range
read_excel(datasets, range = "mtcars!B1:D5")
#> # A tibble: 4 × 3
#>     cyl  disp    hp
#>   <dbl> <dbl> <dbl>
#> 1     6   160   110
#> 2     6   160   110
#> 3     4   108    93
#> 4     6   258   110

# Read only specific rows or columns
read_excel(datasets, range = cell_rows(102:151), col_names = FALSE)
#> # A tibble: 0 × 0
read_excel(datasets, range = cell_cols("B:D"))
#> # A tibble: 32 × 3
#>      cyl  disp    hp
#>    <dbl> <dbl> <dbl>
#>  1     6  160    110
#>  2     6  160    110
#>  3     4  108     93
#>  4     6  258    110
#>  5     8  360    175
#>  6     6  225    105
#>  7     8  360    245
#>  8     4  147.    62
#>  9     4  141.    95
#> 10     6  168.   123
#> # ℹ 22 more rows

# Get a preview of column names
names(read_excel(readxl_example("datasets.xlsx"), n_max = 0))
#>  [1] "mpg"  "cyl"  "disp" "hp"   "drat" "wt"   "qsec" "vs"   "am"  
#> [10] "gear" "carb"

# exploit full .name_repair flexibility from tibble

# "universal" names are unique and syntactic
read_excel(
  readxl_example("deaths.xlsx"),
  range = "arts!A5:F15",
  .name_repair = "universal"
)
#> New names:
#> • `Has kids` -> `Has.kids`
#> • `Date of birth` -> `Date.of.birth`
#> • `Date of death` -> `Date.of.death`
#> # A tibble: 10 × 6
#>    Name               Profession   Age Has.kids Date.of.birth      
#>    <chr>              <chr>      <dbl> <lgl>    <dttm>             
#>  1 David Bowie        musician      69 TRUE     1947-01-08 00:00:00
#>  2 Carrie Fisher      actor         60 TRUE     1956-10-21 00:00:00
#>  3 Chuck Berry        musician      90 TRUE     1926-10-18 00:00:00
#>  4 Bill Paxton        actor         61 TRUE     1955-05-17 00:00:00
#>  5 Prince             musician      57 TRUE     1958-06-07 00:00:00
#>  6 Alan Rickman       actor         69 FALSE    1946-02-21 00:00:00
#>  7 Florence Henderson actor         82 TRUE     1934-02-14 00:00:00
#>  8 Harper Lee         author        89 FALSE    1926-04-28 00:00:00
#>  9 Zsa Zsa Gábor      actor         99 TRUE     1917-02-06 00:00:00
#> 10 George Michael     musician      53 FALSE    1963-06-25 00:00:00
#> # ℹ 1 more variable: Date.of.death <dttm>

# specify name repair as a built-in function
read_excel(readxl_example("clippy.xlsx"), .name_repair = toupper)
#> # A tibble: 4 × 2
#>   NAME                 VALUE    
#>   <chr>                <chr>    
#> 1 Name                 Clippy   
#> 2 Species              paperclip
#> 3 Approx date of death 39083    
#> 4 Weight in grams      0.9      

# specify name repair as a custom function
my_custom_name_repair <- function(nms) tolower(gsub("[.]", "_", nms))
read_excel(
  readxl_example("datasets.xlsx"),
  .name_repair = my_custom_name_repair
)
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

# specify name repair as an anonymous function
read_excel(
  readxl_example("datasets.xlsx"),
  sheet = "chickwts",
  .name_repair = ~ substr(.x, start = 1, stop = 3)
)
#> # A tibble: 71 × 2
#>      wei fee      
#>    <dbl> <chr>    
#>  1   179 horsebean
#>  2   160 horsebean
#>  3   136 horsebean
#>  4   227 horsebean
#>  5   217 horsebean
#>  6   168 horsebean
#>  7   108 horsebean
#>  8   124 horsebean
#>  9   143 horsebean
#> 10   140 horsebean
#> # ℹ 61 more rows
```
