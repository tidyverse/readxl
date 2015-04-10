## Changes

This is a resubmission:

* The DESCRIPTION now includes the individual readxl authors
* I have noted in LICENSE that libxls is licensed with BSD 2-clause
* I have wrapped external package names with single quotes

Please note that the libxls webpage is severly out of date: it describes libxls 0.2.0, where the current (bundled) version is 1.4.0. I don't have an big-endian machine to test the code on, but the code includes endianness checks which leads me to suspect it is supported. Similarly, I have experimentally verified that libxls supports Excel 95 workbooks, despite the claim on the webpage.

## Test environments
* local OS X install, R 3.1.3
* ubuntu 12.04 (on travis-ci), R 3.1.3
* win-builder (devel and release)

## R CMD check results
There were no ERRORs or WARNINGs. 

There was 1 NOTE:

* This is a new submission

## Downstream dependencies
This is a new submission
