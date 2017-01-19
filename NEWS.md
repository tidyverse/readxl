# readxl 0.1.1.9000

* Unwanted printed output (e.g., `DEFINEDNAME: 21 00 00 ...`) is suppressed when reading .xls that contains a defined range, (#82, #188, @PedramNavid).

* Import the `tibble` package (#175, @krlmlr).

# readxl 0.1.1

* Add support for correctly reading strings in .xlsx files containing escaped 
  unicode characters (e.g. `_x005F_`) (#51, thanks to @jmarshallnz).
