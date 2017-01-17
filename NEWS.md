# readxl 0.1.1.9000

* Don't access value of `numFmtId` attribute when it does not exist. Can occur in xlsx written by <http://epplus.codeplex.com/> (#191, #229).

* Import the `tibble` package (#175, @krlmlr).

# readxl 0.1.1

* Add support for correctly reading strings in .xlsx files containing escaped 
  unicode characters (e.g. `_x005F_`) (#51, thanks to @jmarshallnz).
