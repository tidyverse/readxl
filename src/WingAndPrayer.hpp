#include "XlsWorkBook.h"
#include "XlsxWorkBook.h"

#include "cpp11/R.hpp"

#include <string>
#include <vector>

class XlsSheetData {

public:
  XlsSheetData(const XlsWorkBook& wb, int sheet_i)
    : pos_(sheet_i)
  {
    Rprintf("XlsSheetData() constructor\n");
  }

  int pos() const { return pos_; }

private:
  int pos_;
};

class XlsxSheetData {

public:
  XlsxSheetData(const XlsxWorkBook& wb, int sheet_i)
    : pos_(sheet_i)
  {
    Rprintf("XlsxSheetData() constructor\n");
  }

  int pos() const { return pos_; }

private:
  int pos_;
};

class XlsCell {};
class XlsxCell {};

class Xls {
public:
 typedef XlsWorkBook  Book;
 typedef XlsSheetData SheetData;
 typedef XlsCell      Cell;
};

class Xlsx {
public:
  typedef XlsxWorkBook  Book;
  typedef XlsxSheetData SheetData;
  typedef XlsxCell      Cell;
};

template <typename T>
class Sheet {

public:
 Sheet(const std::string& path,
       int sheet_i)
  : wb_(path),
    sd_(wb_, sheet_i)
 {
   Rprintf("Sheet() constructor\n");
   Rprintf("Path to this Sheet's Book: %s\n", wb_.path().c_str());
   Rprintf("This Sheet's SheetData pos: %d\n", sd_.pos());
 }

 void commonMethod() {
  Rprintf("I'm the common Sheet method!\n");
  Rprintf("Path to this Sheet's Book: %s\n", wb_.path().c_str());
 }

private:
 typename T::Book wb_;
 typename T::SheetData sd_;
 //std::vector<typename T::Cell> cells_;
};

template <>
void Sheet<Xlsx>::commonMethod() {
  Rprintf("I'm doing something special for xlsx Sheets!\n");
  Rprintf("Path to xlsx: %s\n", wb_.path().c_str());
}
