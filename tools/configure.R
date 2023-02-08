# Note from @gaborcsardi's review: Since we modify Makevars and Makevars.win in
# place, if someone does multiple installs from the same directory (as opposed
# to a package tarball), subsequent installs could get modified Makevars, even
# if that's not appropriate. This seems pretty unlikely to come up in this case.
if (getRversion() < '3.6') {
  makevars <- file.path('src', 'Makevars')
  makevars_win <- file.path('src', 'Makevars.win')

  txt <- readLines(makevars)
  txt_win <- readLines(makevars_win)

  if (!any(grepl("^CXX_STD", txt))) {
    txt <- c("CXX_STD = CXX11", txt)
    cat(txt, file = makevars, sep = "\n")
  }

  if (!any(grepl("^CXX_STD", txt_win))) {
    txt_win <- c("CXX_STD = CXX11", txt_win)
    cat(txt_win, file = makevars_win, sep = "\n")
  }
}
