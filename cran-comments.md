This is a patch release in response to a 2023-06-28 email from Kurt Hornik about inputs to numeric_version() and package_version().

In this case, the problematic inputs came from implicit usage via `>` in some tests and these have been removed.

## revdepcheck results

We checked 319 reverse dependencies, comparing R CMD check results across CRAN and dev versions of this package.

 * We saw 0 new problems
 * We failed to check 0 packages
