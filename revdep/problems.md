# readmoRe

<details>

* Version: 0.2-12
* GitHub: NA
* Source code: https://github.com/cran/readmoRe
* Date/Publication: 2021-08-19 07:50:02 UTC
* Number of recursive dependencies: 26

Run `revdepcheck::cloud_details(, "readmoRe")` for more info

</details>

## Newly broken

*   checking examples ... ERROR
    ```
    Running examples in ‘readmoRe-Ex.R’ failed
    The error most likely occurred in:
    
    > ### Name: read.to.list
    > ### Title: Read various input file formats into a list of data frames.
    > ###   Wrapper function for 'read2list' to automate reading further and
    > ###   avoid errors due to missing folders or files.
    > ### Aliases: read.to.list
    > ### Keywords: utilities
    > 
    ...
    > # All example data are read into a list. From the Excel file, sheets 1 and
    > # 4 are read.
    > dat <- read.to.list(c(readxl_datasets, tsv_datasets), sheet=c(1, 4))
    @ MULTI DATA LOADER
    @ VERSATILE FILE READER v.0.2.12
    Reading Excel file datasets.xlsx...
     Reading sheet(s) number 1,4...
      Sheet 1 read
    Error: Can't retrieve sheet in position 4, only 3 sheet(s) found.
    Execution halted
    ```

## In both

*   checking Rd files ... NOTE
    ```
    checkRd: (-1) get.skip.Rd:15: Lost braces
        15 | line in the file to determine the {code{skip}} value.}
           |                                   ^
    checkRd: (-1) get.skip.Rd:15: Lost braces
        15 | line in the file to determine the {code{skip}} value.}
           |                                        ^
    ```

