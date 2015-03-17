# exell

[![Travis-CI Build Status](https://travis-ci.org/hadley/exell.png?branch=master)](https://travis-ci.org/hadley/exell)

The exell package makes it easy to get data out of Excel and into R. Why exell?

* Compared to the existing packages (e.g. gdata, xlsx, xlsReadWrite etc) exell
  has no external dependencies so it's easy to install and use on all operating 
  systems. It supports both the legacy `.xls` format and the more modern
  `.xlsx` format.

* Compared to opening Excel and saving as csv:

  * Avoids one step with potential for errors (i.e. you save the csv file
    in the wrong place) and it's faster since you don't need to click
    through all warnings.

  * Exell correctly handles non-ASCII characters.
  
  * Reads in dates as dates!

