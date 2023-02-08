if (getRversion() < '3.6') {
  makevars <- file.path('src', 'Makevars')
  makevars_win <- file.path('src', 'Makevars.win')

  txt <- readLines(makevars)
  txt_win <- readLines(makevars_win)

  txt <- c("CXX_STD = CXX11", txt)
  txt_win <- c("CXX_STD = CXX11", txt_win)

  cat(txt, file = makevars, sep = "\n")
  cat(txt_win, file = makevars_win, sep = "\n")
}
