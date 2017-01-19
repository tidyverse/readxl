# readxl 0.1.1.9000

* Unwanted printed output (e.g., `DEFINEDNAME: 21 00 00 ...`) is suppressed when reading .xls that contains a defined range, (#82, #188, @PedramNavid).

* Don't access value of `numFmtId` attribute when it does not exist. Can occur in xlsx written by <http://epplus.codeplex.com/> (#191, #229).

* Import the `tibble` package (#175, @krlmlr).

# readxl 0.1.1

* Add support for correctly reading strings in .xlsx files containing escaped 
  unicode characters (e.g. `_x005F_`) (#51, thanks to @jmarshallnz).

* Bumped up row inspection for column typing guessing from 100 to 1000 to align with readr (#143).
