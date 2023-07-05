# rb3

<details>

* Version: 0.0.10
* GitHub: https://github.com/ropensci/rb3
* Source code: https://github.com/cran/rb3
* Date/Publication: 2023-04-14 12:10:02 UTC
* Number of recursive dependencies: 120

Run `revdepcheck::cloud_details(, "rb3")` for more info

</details>

## Newly broken

*   checking re-building of vignette outputs ... ERROR
    ```
    Error(s) in re-building vignettes:
      ...
    --- re-building ‘B3-Indexes.Rmd’ using rmarkdown
    
    Quitting from lines 72-73 [unnamed-chunk-7] (B3-Indexes.Rmd)
    Error: processing vignette 'B3-Indexes.Rmd' failed with diagnostics:
    attempt to set 'colnames' on an object with less than two dimensions
    --- failed re-building ‘B3-Indexes.Rmd’
    
    --- re-building ‘Fetching-historical-future-rates.Rmd’ using rmarkdown
    ...
    --- finished re-building ‘Fetching-historical-future-rates.Rmd’
    
    --- re-building ‘Fetching-historical-yield-curve.Rmd’ using rmarkdown
    --- finished re-building ‘Fetching-historical-yield-curve.Rmd’
    
    SUMMARY: processing the following file failed:
      ‘B3-Indexes.Rmd’
    
    Error: Vignette re-building failed.
    Execution halted
    ```

