# readxl Webinar

Notes made in preparation for a [readxl RStudio webinar](https://www.rstudio.com/resources/webinars/) given on 2017-05-10. A recording can be found at that link. Slides can be found on [SpeakerDeck](https://speakerdeck.com/jennybc/readxl-rstudio-webinar). Slides and more can be found in the [RStudio Webinar GitHub repo](https://github.com/rstudio/webinars/tree/master/36-readxl#readme) or in [readxl's GitHub repo](https://github.com/tidyverse/readxl/tree/master/talks#readme).

## Description

Like it or not, spreadsheets are a common data source for many of us. We’ll review the overall landscape for importing spreadsheet data into R and then go into detail for the readxl package specifically. [readxl](http://readxl.tidyverse.org) is the [Tidyverse](http://tidyverse.org) solution for reading data stored in the legacy xls format or the more modern xlsx format. It has no tricky external dependencies, is quite speedy, and is easy to install and use across all operating systems.

## Context: Tidyverse

<http://tidyverse.org>

readxl is the Excel-reading package with an interface that is most consistent with, e.g., readr, haven, etc. in terms of data import. Why does this matter?

  * Better transfer of expertise across packages
  * Better recall of how things work when returning after a break
  * Best preparation for downstream work with tibble, tidyr, dplyr, purrr, ggplot2
  
Key concept: once you get data into R, it's stored as a [tibble](http://tibble.tidyverse.org/reference/tibble.html), which is a special flavor of data frame.

## Context: R packages for Excel

There are many R packages that can read Excel spreadsheets besides readxl, such as:

  * [`openxlsx`](http://cran.r-project.org/package=openxlsx), [`XLConnect`](http://cran.r-project.org/package=XLConnect), [`xlsx`](http://cran.r-project.org/package=xlsx),
[`gdata`](http://cran.r-project.org/package=gdata), [`RODBC`](http://cran.r-project.org/package=RODBC), [`excel.link`](http://cran.r-project.org/package=excel.link), and more!

Why would you pick readxl?

  * Consistency with tidyverse ingest conventions (see previous section)
  * Reads both `.xls` and `.xlsx` (see next section)
  * No aggravating external dependencies (see section after that)
  * Common user experience across Mac, Windows, Linux (ditto)

### Excel file formats

`.xls` = legacy Excel format, Excel '97(-2007)

  * binary
  * you do not want to parse this yourself! stand on the shoulders of others!
  * ergo, widespread reliance of R packages on external solutions

`.xlsx` = modern Excel format

  * "just" a zip archive of a bunch of XML files. Go ahead! Unzip it and visit the unpacked XML files in your favorite browser.
  * you also do not want to parse XML yourself! stand on the shoulders of others!
  * ergo, widespread reliance of R packages on external solutions
  
**Pick a package that reads the format(s) you have.**

### Aggravating external dependencies

All Excel-reading R packages are relying on external libraries. The only question is whether the user will feel that or not.

readxl benefits from

  * [libxls](https://sourceforge.net/projects/libxls/) for parsing `.xls` files
  * [RapidXML](http://rapidxml.sourceforge.net) for parsing the XML inside `.xlsx`

readxl fully embeds these libraries. On Mac, Windows, and Linux, you should be able to just install readxl and go.

The alternative: get the user to install external dependencies.

  * Good news: [Apache POI](https://poi.apache.org) is "the Java API for Microsoft Documents" and it is the most comprehensive solution for reading MS Excel files. It's as close as you'll ever get to: "if Excel can read it, this thing can read it".
    - [`XLConnect`](http://cran.r-project.org/package=XLConnect) and [`xlsx`](http://cran.r-project.org/package=xlsx) use Apache POI
  * Bad news: Java. In my experience teaching classroms of ~80 with ~50/50 mix of Mac and Windows, ~30% of students can't get this working immediately on their own. The packages don't merely work poorly or slowly for these students; they simply do not work at all. I'm sure most of these problems can be solved with personalized expert help, but I can't offer that.
  * A similar scenario is the Perl dependency of [`gdata`](http://cran.r-project.org/package=gdata).

## readxl: reading rectangles

readxl targets rectangular data in spreadsheets.

### Which rectangle?

Draw on these resources:

  * "Specifying the data rectangle", [readxl 1.0.0 RStudio blog post](https://blog.rstudio.org/2017/04/19/readxl-1-0-0/)
  * [Sheet Geometry vignette](http://readxl.tidyverse.org/articles/sheet-geometry.html)
  * RStudio IDE support for readxl
  
#### RStudio IDE support

Access the RStudio helper in one of two ways:

  * Environment pane > Import Dataset > From Excel...
  * Files > click on `.xls` or `.xlsx` > Import Dataset...
  
What's cool about it?

  * Import options expose the most important arguments of `read_excel()`, such as:
    - `skip`, `n_max`, or `range`
  * Data preview helps you see the implications of your choices, especially re: column typing.
  * Code Preview and clipboard feature help you see and capture the necessary code.
  * Has better support for reading `.xls` and `.xlsx` from a URL than readxl does at the moment

Bear in mind there are more arguments to `read_excel()` and more flexible ways to use these arguments if you write the code yourself.

### Column types

Draw on these resources:

  * "Column typing", [readxl 1.0.0 RStudio blog post](https://blog.rstudio.org/2017/04/19/readxl-1-0-0/)
  * [Column Types vignette](http://readxl.tidyverse.org/articles/cell-and-column-types.html)

## Workflows and iteration

Draw on <http://readxl.tidyverse.org/articles/articles/readxl-workflows.html>

