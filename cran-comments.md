This is a patch release in response to correspondence from Prof Brian Ripley (original deadline Feb 13) and Kurt Hornik (extending deadline to Feb 27). This release:

* Removes the usage of `Rf_StringFalse` and `Rf_StringTrue`
* Addresses the issue seen with gcc 15
* Update the embedded libxls library to fix some vulnerabilities
* Fixes the NOTE about an Rd \link{} with missing package anchor

This release does not address:

* Compilation warning about "whitespace in a literal operator declaration", which is a known upstream issue in cpp11.
* An "Additional issue" that has recently appeared in gcc-UBSAN. I'm still analyzing this.

## revdepcheck results

We checked 398 reverse dependencies, comparing R CMD check results across CRAN and dev versions of this package.

 * We saw 1 new problems
 * We failed to check 1 packages

Issues with CRAN packages are summarised below.

### New problems

* readmoRe
  checking examples ...
  Error: Can't retrieve sheet in position 4, only 3 sheet(s) found.

  This example fails because a spreadsheet that ships with readxl now has one
  less sheet than previously.
  This package is not maintained on GitHub, so I can't send a pull request, but
  I have sent the maintainer an email with a suggestion of how to fix.

### Failed to check

* TestGenerator (NA)
  Fails with "Error: object ‘eunomia_dir’ is not exported by 'namespace:CDMConnector'"
