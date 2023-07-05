# datplot

<details>

* Version: 1.0.0
* GitHub: https://github.com/lsteinmann/datplot
* Source code: https://github.com/cran/datplot
* Date/Publication: 2021-03-04 10:00:05 UTC
* Number of recursive dependencies: 126

Run `revdepcheck::cloud_details(, "datplot")` for more info

</details>

## Newly broken

*   checking tests ... ERROR
    ```
      Running ‘create_testing_df.R’
      Running ‘testthat.R’
    Running the tests in ‘tests/testthat.R’ failed.
    Last 13 lines of output:
      [1] "stepsize has to be either 'auto' or numeric."
      [ FAIL 1 | WARN 0 | SKIP 0 | PASS 21 ]
      
      ══ Failed tests ════════════════════════════════════════════════════════════════
      ── Error ('test-datplot_utility.R:27:3'): (code run outside of `test_that()`) ──
      Error in `seq.default(from = datmin, to = datmax, by = stepsize)`: wrong sign in 'by' argument
      Backtrace:
          ▆
       1. └─datplot::get.step.sequence(...) at test-datplot_utility.R:27:2
       2.   ├─base::seq(from = datmin, to = datmax, by = stepsize)
       3.   └─base::seq.default(from = datmin, to = datmax, by = stepsize)
      
      [ FAIL 1 | WARN 0 | SKIP 0 | PASS 21 ]
      Error: Test failures
      Execution halted
    ```

## In both

*   checking data for non-ASCII characters ... NOTE
    ```
      Note: found 1 marked Latin-1 string
      Note: found 360 marked UTF-8 strings
    ```

