#include "cpp11/R.hpp"

#include <string>
#include <vector>

class XlsBook {

public:
 XlsBook(const std::string& path)
  : path_(path)
 {
   Rprintf("XlsBook() constructor\n");
 }

 std::string path() const { return path_; }

private:
 std::string path_;

};

class XlsxBook {

public:
 XlsxBook(const std::string& path)
  : path_(path)
 {
   Rprintf("XlsxBook() constructor\n");
 }

 std::string path() const { return path_; }

private:
 std::string path_;

};

class XlsSheet {
};

class XlsxSheet {
};

class XlsCell {};
class XlsxCell {};

class Xls {
public:
 typedef XlsBook  Book;
 typedef XlsSheet Sheet;
 typedef XlsCell  Cell;
};

class Xlsx {
public:
 typedef XlsxBook  Book;
 typedef XlsxSheet Sheet;
 typedef XlsxCell  Cell;
};

template <typename T>
class Sheet {

public:
 Sheet(const std::string& path)
  : wb_(path)
 {
   Rprintf("Sheet() constructor\n");
 }

 void commonMethod() {
  Rprintf("I'm the common Sheet method!\n");
  Rprintf("Path to this Sheet's Book: %s\n", wb_.path().c_str());
 }

private:
 typename T::Book wb_;
 std::vector<typename T::Cell> cells_;
};

template <>
void Sheet<Xlsx>::commonMethod() {
  Rprintf("I'm doing something special for xlsx Sheets!\n");
  Rprintf("Path to xlsx: %s\n", wb_.path().c_str());
}
