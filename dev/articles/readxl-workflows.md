# readxl Workflows

Ideas for using readxl to increase reproducibility and reduce tedium.

Reproducibility is much easier in theory than in reality. Here are some
special dilemmas we face with spreadsheets:

- We say: Keep raw data raw! But we also say: Store data in a
  tool-agnostic, future-proof format! If data comes in the form of an
  `.xls[x]` file, we’re in a pickle. The `.xls[x]` file should obviously
  be preserved, and probably write-protected. But a faithful copy as CSV
  is a wonderful complement, as long as you can ensure the two are the
  same.
- We say: Don’t do analysis “by hand” or “by mouse”! But then we break
  this rule by manually exporting spreadsheets to file with *File \>
  Save As … \> (save to `.csv`)*. readxl helps you get data directly out
  of a spreadsheet and into R, where you can record every step of your
  analysis as code. Below we show how to cache a CSV snapshot as part of
  this process.

The examples below also demonstrate the use of functional programming or
“apply” techniques to iterate over the worksheets in a workbook.

## Load packages

We load the tidyverse metapackage here because the workflows below show
readxl working with readr, purrr, etc. See the last section for
solutions using base R only (other than readxl).

We must load readxl explicitly because it is not part of the core
tidyverse.

``` r

library(tidyverse)
#> ── Attaching core tidyverse packages ─────────────── tidyverse 2.0.0 ──
#> ✔ dplyr     1.2.1     ✔ readr     2.2.0
#> ✔ forcats   1.0.1     ✔ stringr   1.6.0
#> ✔ ggplot2   4.0.3     ✔ tibble    3.3.1
#> ✔ lubridate 1.9.5     ✔ tidyr     1.3.2
#> ✔ purrr     1.2.2     
#> ── Conflicts ───────────────────────────────── tidyverse_conflicts() ──
#> ✖ dplyr::filter() masks stats::filter()
#> ✖ dplyr::lag()    masks stats::lag()
#> ℹ Use the conflicted package (<http://conflicted.r-lib.org/>) to force all conflicts to become errors
library(readxl)
```

## Cache a CSV snapshot

Break analyses into logical steps, via a series of scripts that relate
to one theme, such as “clean the data” or “make exploratory and
diagnostic plots”.

This forces you to transmit info from step i to step i + 1 via a set of
output files. The cumulative outputs of steps 1, 2, …, i are available
as inputs for steps i + 1 and beyond.

These outputs constitute [an
API](https://www.quora.com/What-is-an-API-4) for your analysis,
i.e. they provide clean entry points that can be used (and understood)
in isolation, possibly using an entirely different toolkit. Contrast
this with the alternative of writing one monolithic script or
transmitting entire workspaces via
[`save()`](https://rdrr.io/r/base/save.html),
[`load()`](https://rdrr.io/r/base/load.html), and R-specific `.rds`
files.

If raw data is stored *only* as an Excel spreadsheet, this limits your
ability to inspect it when solving the little puzzles that crop up in
dowstream work. You’ll need to fire up Excel (or similar) and get busy
with your mouse. You certainly can’t poke around it or view diffs on
GitHub.

Solution: cache a CSV snapshot of your raw data tables at the time of
export. Even if you use
[`read_excel()`](https://readxl.tidyverse.org/dev/reference/read_excel.md)
for end-to-end reproducibility, this complementary CSV leaves your
analysis in a more accessible state.

Pipe the output of
[`read_excel()`](https://readxl.tidyverse.org/dev/reference/read_excel.md)
directly into
[`readr::write_csv()`](https://readr.tidyverse.org/reference/write_delim.html)
like so:

``` r

mtcars_xl <- readxl_example("datasets.xlsx") |>
  read_excel(sheet = "mtcars") |>
  write_csv("mtcars-raw.csv")
```

Why does this work?
[`readr::write_csv()`](https://readr.tidyverse.org/reference/write_delim.html)
is a well-mannered “write” function: it does its main job *and returns
its input invisibly*. The above command reads the mtcars sheet from
readxl’s `datasets.xlsx` example workbook and caches a CSV version of
the resulting data frame to file.

Let’s check. Did we still import the data? Did we write the CSV file?

``` r

mtcars_xl
#> # A tibble: 32 × 11
#>     mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#>   <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#> 1  21       6   160   110  3.9   2.62  16.5     0     1     4     4
#> 2  21       6   160   110  3.9   2.88  17.0     0     1     4     4
#> 3  22.8     4   108    93  3.85  2.32  18.6     1     1     4     1
#> # ℹ 29 more rows
dir(pattern = "mtcars")
#> [1] "mtcars-raw.csv"
```

Yes! Is the data written to CSV an exact copy of what we imported from
Excel?

``` r

mtcars_alt <- read_csv("mtcars-raw.csv")
#> Rows: 32 Columns: 11
#> ── Column specification ───────────────────────────────────────────────
#> Delimiter: ","
#> dbl (11): mpg, cyl, disp, hp, drat, wt, qsec, vs, am, gear, carb
#> 
#> ℹ Use `spec()` to retrieve the full column specification for this data.
#> ℹ Specify the column types or set `show_col_types = FALSE` to quiet this message.
## readr leaves a note-to-self in `spec` that records its column guessing,
## so we remove that attribute before the check
attr(mtcars_alt, "spec") <- NULL
identical(mtcars_xl, mtcars_alt)
#> [1] FALSE
```

Yes! If we needed to restart or troubleshoot this fictional analysis,
`mtcars-raw.csv` is available as a second, highly accessible alternative
to `datasets.xlsx`.

## Iterate over multiple worksheets in a workbook

Some Excel workbooks contain only data and you are tempted to ask “Why,
God, why is this data stored in Excel? Why not store this as a series of
CSV files?” One possible answer is this: because the workbook structure
keeps them all together.

Let’s accept that this happens and that it is not entirely crazy. How
can you efficiently load all of that into R?

Here’s how to load all the sheets in a workbook into a list of data
frames:

- Get worksheet names as a self-named character vector (these names
  propagate nicely).
- Use [`purrr::map()`](https://purrr.tidyverse.org/reference/map.html)
  to iterate sheet reading.

``` r

path <- readxl_example("datasets.xlsx")
path |>
  excel_sheets() |>
  set_names() |>
  map(read_excel, path = path)
#> $mtcars
#> # A tibble: 32 × 11
#>     mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#>   <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#> 1  21       6   160   110  3.9   2.62  16.5     0     1     4     4
#> 2  21       6   160   110  3.9   2.88  17.0     0     1     4     4
#> 3  22.8     4   108    93  3.85  2.32  18.6     1     1     4     1
#> # ℹ 29 more rows
#> 
#> $chickwts
#> # A tibble: 71 × 2
#>   weight feed     
#>    <dbl> <chr>    
#> 1    179 horsebean
#> 2    160 horsebean
#> 3    136 horsebean
#> # ℹ 68 more rows
#> 
#> $quakes
#> # A tibble: 1,000 × 5
#>     lat  long depth   mag stations
#>   <dbl> <dbl> <dbl> <dbl>    <dbl>
#> 1 -20.4  182.   562   4.8       41
#> 2 -20.6  181.   650   4.2       15
#> 3 -26    184.    42   5.4       43
#> # ℹ 997 more rows
```

## CSV caching and iterating over sheets

What if we want to read all the sheets in at once and simultaneously
cache to CSV? We define `read_then_csv()` as
`read_excel(...) |> write_csv()` and use
[`purrr::map()`](https://purrr.tidyverse.org/reference/map.html) again.

``` r

read_then_csv <- function(sheet, path) {
  pathbase <- path |>
    basename() |>
    tools::file_path_sans_ext()
  path |>
    read_excel(sheet = sheet) |>
    write_csv(paste0(pathbase, "-", sheet, ".csv"))
}
```

We could even define this on-the-fly as an anonymous function inside
[`map()`](https://purrr.tidyverse.org/reference/map.html), but I think
this is more readable.

``` r

path <- readxl_example("datasets.xlsx")
path |>
  excel_sheets() |>
  set_names() |>
  map(read_then_csv, path = path)
#> $mtcars
#> # A tibble: 32 × 11
#>     mpg   cyl  disp    hp  drat    wt  qsec    vs    am  gear  carb
#>   <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl> <dbl>
#> 1  21       6   160   110  3.9   2.62  16.5     0     1     4     4
#> 2  21       6   160   110  3.9   2.88  17.0     0     1     4     4
#> 3  22.8     4   108    93  3.85  2.32  18.6     1     1     4     1
#> # ℹ 29 more rows
#> 
#> $chickwts
#> # A tibble: 71 × 2
#>   weight feed     
#>    <dbl> <chr>    
#> 1    179 horsebean
#> 2    160 horsebean
#> 3    136 horsebean
#> # ℹ 68 more rows
#> 
#> $quakes
#> # A tibble: 1,000 × 5
#>     lat  long depth   mag stations
#>   <dbl> <dbl> <dbl> <dbl>    <dbl>
#> 1 -20.4  182.   562   4.8       41
#> 2 -20.6  181.   650   4.2       15
#> 3 -26    184.    42   5.4       43
#> # ℹ 997 more rows
dir(pattern = "^datasets.*\\.csv$")
#> [1] "datasets-chickwts.csv" "datasets-mtcars.csv"  
#> [3] "datasets-quakes.csv"
```

In a real analysis, starting with workbook `"foo.xlsx"`, you might want
to create the directory `foo` and place the CSVs inside that.

## Concatenate worksheets into one data frame

What if the datasets found on different sheets have the same variables?
Then you’ll want to row-bind them, after import, to form one big,
beautiful data frame.

readxl ships with an example sheet `deaths.xlsx`, containing data on
famous people who died in 2016 or 2017. It has two worksheets named
“arts” and “other”, but the spreadsheet layout is the same in each and
the data tables have the same variables, e.g., name and date of death.

The [`map()`](https://purrr.tidyverse.org/reference/map.html) function
from purrr makes it easy to iterate over worksheets. Use
[`purrr::list_rbind()`](https://purrr.tidyverse.org/reference/list_c.html)
to glue together the resulting data frames.

- Store a self-named vector of worksheet names (critical for the ID
  variable below).
- Use `purrr::map() |> purrr::list_rbind()` to import the data, create
  an ID variable for the source worksheet, and row bind.

``` r

path <- readxl_example("deaths.xlsx")
deaths <- path |>
  excel_sheets() |>
  set_names() |>
  map(~ read_excel(path = path, sheet = .x, range = "A5:F15")) |>
  list_rbind(names_to = "sheet")
print(deaths, n = Inf)
#> # A tibble: 20 × 7
#>    sheet Name           Profession   Age `Has kids` `Date of birth`    
#>    <chr> <chr>          <chr>      <dbl> <lgl>      <dttm>             
#>  1 arts  David Bowie    musician      69 TRUE       1947-01-08 00:00:00
#>  2 arts  Carrie Fisher  actor         60 TRUE       1956-10-21 00:00:00
#>  3 arts  Chuck Berry    musician      90 TRUE       1926-10-18 00:00:00
#>  4 arts  Bill Paxton    actor         61 TRUE       1955-05-17 00:00:00
#>  5 arts  Prince         musician      57 TRUE       1958-06-07 00:00:00
#>  6 arts  Alan Rickman   actor         69 FALSE      1946-02-21 00:00:00
#>  7 arts  Florence Hend… actor         82 TRUE       1934-02-14 00:00:00
#>  8 arts  Harper Lee     author        89 FALSE      1926-04-28 00:00:00
#>  9 arts  Zsa Zsa Gábor  actor         99 TRUE       1917-02-06 00:00:00
#> 10 arts  George Michael musician      53 FALSE      1963-06-25 00:00:00
#> 11 other Vera Rubin     scientist     88 TRUE       1928-07-23 00:00:00
#> 12 other Mohamed Ali    athlete       74 TRUE       1942-01-17 00:00:00
#> 13 other Morley Safer   journalist    84 TRUE       1931-11-08 00:00:00
#> 14 other Fidel Castro   politician    90 TRUE       1926-08-13 00:00:00
#> 15 other Antonin Scalia lawyer        79 TRUE       1936-03-11 00:00:00
#> 16 other Jo Cox         politician    41 TRUE       1974-06-22 00:00:00
#> 17 other Janet Reno     lawyer        78 FALSE      1938-07-21 00:00:00
#> 18 other Gwen Ifill     journalist    61 FALSE      1955-09-29 00:00:00
#> 19 other John Glenn     astronaut     95 TRUE       1921-07-28 00:00:00
#> 20 other Pat Summit     coach         64 TRUE       1952-06-14 00:00:00
#> # ℹ 1 more variable: `Date of death` <dttm>
```

Note the use of `range = "A5:E15"` here. `deaths.xlsx` is a typical
spreadsheet and includes a few non-data lines at the top and bottom and
this argument specifies where the data rectangle lives.

## Putting it all together

All at once now:

- Multiple worksheets feeding into one data frame
- Sheet-specific target rectangles
- Cache to CSV upon import

Even though the worksheets in `deaths.xlsx` have the same layout, we’ll
pretend they don’t and specify the target rectangle in two different
ways here. This shows how this can work if each worksheet has it’s own
peculiar geometry. Here’s the workflow:

- Store a self-named vector of worksheet names.
- Store a vector of cell range specifications.
- Use `purrr::map2() |> purrr::list_rbind()` to iterate over those two
  vectors in parallel, importing the data, row binding, and creating an
  ID variable for the source worksheet.
- Cache the unified data to CSV.

``` r

path <- readxl_example("deaths.xlsx")
sheets <- path |>
  excel_sheets() |>
  set_names()
ranges <- list("A5:F15", cell_rows(5:15))
deaths <- map2(
  sheets,
  ranges,
  ~ read_excel(path, sheet = .x, range = .y)
) |>
  list_rbind(names_to = "sheet") |>
  write_csv("deaths.csv")
print(deaths, n = Inf)
#> # A tibble: 20 × 7
#>    sheet Name           Profession   Age `Has kids` `Date of birth`    
#>    <chr> <chr>          <chr>      <dbl> <lgl>      <dttm>             
#>  1 arts  David Bowie    musician      69 TRUE       1947-01-08 00:00:00
#>  2 arts  Carrie Fisher  actor         60 TRUE       1956-10-21 00:00:00
#>  3 arts  Chuck Berry    musician      90 TRUE       1926-10-18 00:00:00
#>  4 arts  Bill Paxton    actor         61 TRUE       1955-05-17 00:00:00
#>  5 arts  Prince         musician      57 TRUE       1958-06-07 00:00:00
#>  6 arts  Alan Rickman   actor         69 FALSE      1946-02-21 00:00:00
#>  7 arts  Florence Hend… actor         82 TRUE       1934-02-14 00:00:00
#>  8 arts  Harper Lee     author        89 FALSE      1926-04-28 00:00:00
#>  9 arts  Zsa Zsa Gábor  actor         99 TRUE       1917-02-06 00:00:00
#> 10 arts  George Michael musician      53 FALSE      1963-06-25 00:00:00
#> 11 other Vera Rubin     scientist     88 TRUE       1928-07-23 00:00:00
#> 12 other Mohamed Ali    athlete       74 TRUE       1942-01-17 00:00:00
#> 13 other Morley Safer   journalist    84 TRUE       1931-11-08 00:00:00
#> 14 other Fidel Castro   politician    90 TRUE       1926-08-13 00:00:00
#> 15 other Antonin Scalia lawyer        79 TRUE       1936-03-11 00:00:00
#> 16 other Jo Cox         politician    41 TRUE       1974-06-22 00:00:00
#> 17 other Janet Reno     lawyer        78 FALSE      1938-07-21 00:00:00
#> 18 other Gwen Ifill     journalist    61 FALSE      1955-09-29 00:00:00
#> 19 other John Glenn     astronaut     95 TRUE       1921-07-28 00:00:00
#> 20 other Pat Summit     coach         64 TRUE       1952-06-14 00:00:00
#> # ℹ 1 more variable: `Date of death` <dttm>
```

## Base version

Rework examples from above but using base R only, other than readxl.

### Cache a CSV snapshot

``` r

mtcars_xl <- read_excel(readxl_example("datasets.xlsx"), sheet = "mtcars")
write.csv(mtcars_xl, "mtcars-raw.csv", row.names = FALSE, quote = FALSE)
mtcars_alt <- read.csv("mtcars-raw.csv", stringsAsFactors = FALSE)
## coerce mtcars_xl back to a data.frame
identical(as.data.frame(mtcars_xl), mtcars_alt)
```

### Iterate over multiple worksheets in a workbook

``` r

path <- readxl_example("datasets.xls")
sheets <- excel_sheets(path)
xl_list <- lapply(excel_sheets(path), read_excel, path = path)
names(xl_list) <- sheets
```

### CSV caching and iterating over sheets

``` r

read_then_csv <- function(sheet, path) {
  pathbase <- tools::file_path_sans_ext(basename(path))
  df <- read_excel(path = path, sheet = sheet)
  write.csv(df, paste0(pathbase, "-", sheet, ".csv"),
            quote = FALSE, row.names = FALSE)
  df
}
path <- readxl_example("datasets.xlsx")
sheets <- excel_sheets(path)
xl_list <- lapply(excel_sheets(path), read_then_csv, path = path)
names(xl_list) <- sheets
```

### Concatenate worksheets into one data frame

``` r

path <- readxl_example("deaths.xlsx")
sheets <- excel_sheets(path)
xl_list <-
  lapply(excel_sheets(path), read_excel, path = path, range = "A5:F15")
xl_list <- lapply(seq_along(sheets), function(i) {
  data.frame(sheet = I(sheets[i]), xl_list[[i]])
})
xl_list <- do.call(rbind, xl_list)
```

### Putting it all together

``` r

path <- readxl_example("deaths.xlsx")
sheets <- excel_sheets(path)
ranges <- list("A5:F15", cell_rows(5:15))
xl_list <- mapply(function(x, y) {
  read_excel(path = path, sheet = x, range = y)
}, sheets, ranges, SIMPLIFY = FALSE)
xl_list <- lapply(seq_along(sheets), function(i) {
  data.frame(sheet = I(sheets[i]), xl_list[[i]])
})
xl_list <- do.call(rbind, xl_list)
write.csv(xl_list, "deaths.csv", row.names = FALSE, quote = FALSE)
```
