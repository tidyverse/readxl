# Determine whether to show progress spinner

By default, readxl displays a progress spinner **unless** one of the
following is `TRUE`:

- The spinner is explicitly disabled by setting
  `options(readxl.show_progress = FALSE)`.

- The code is run in a non-interactive session
  ([`interactive()`](https://rdrr.io/r/base/interactive.html) is
  `FALSE`).

- The code is run by knitr / rmarkdown.

- The code is run in an RStudio notebook chunk. readxl uses the
  [progress package](https://cran.r-project.org/package=progress)
  under-the-hood and therefore is also sensitive to any options that it
  consults.

## Usage

``` r
readxl_progress()
```
