#ifndef READXL_XLSWORKBOOK_
#define READXL_XLSWORKBOOK_

#include <sstream>
#include "ColSpec.h"
#include "libxls/xls.h"

class XlsWorkBook {

  // common to Xls[x]WorkBook
  std::string path_;
  bool is1904_;
  std::set<int> dateFormats_;

  // kept as data + accessor in XlsWorkBook vs. member function in XlsxWorkBook
  int n_sheets_;
  cpp11::writable::strings sheets_;

public:

  XlsWorkBook(const std::string& path) {
    path_ = path;

    xls::xls_error_t error = xls::LIBXLS_OK;
    xls::xlsWorkBook* pWB_ = xls::xls_open_file(path_.c_str(), "UTF-8", &error);
    if (!pWB_) {
      auto err = xls::xls_getError(error);
      std::stringstream ss;
      ss << "\n filepath: " << path_ << "\n libxls error: " << err;
      throw std::runtime_error(ss.str());
    }

    n_sheets_ = pWB_->sheets.count;
    sheets_ = cpp11::writable::strings(n_sheets());
    for (int i = 0; i < n_sheets_; ++i) {
      sheets_[i] = Rf_mkCharCE((char*) pWB_->sheets.sheet[i].name, CE_UTF8);
    }

    is1904_ = pWB_->is1904;
    cacheDateFormats(pWB_);

    xls::xls_close_WB(pWB_);
  }

  const std::string& path() const{
    return path_;
  }

  int n_sheets() const {
    return n_sheets_;
  }

  cpp11::strings sheets() const {
    return sheets_;
  }

  bool is1904() const {
    return is1904_;
  }

  const std::set<int>& dateFormats() const {
    return dateFormats_;
  }

private:

  void cacheDateFormats(xls::xlsWorkBook* pWB) {

    // Figure out which custom formats are dates
    // [MS-XLS] 2.4.126 Format p301
    std::set<int> customDateFormats;
    int n_formats = pWB->formats.count;
    if (n_formats > 0) {
      for (int i = 0; i < n_formats; ++i) {
        const xls::st_format::st_format_data format = pWB->formats.format[i];
        // format.value = format string
        // in xlsx, this is formatCode
        std::string code((char*) format.value);
        if (isDateFormat(code)) {
          // format.index = identifier used by other records
          // in xlsx, this is numFormatId
          customDateFormats.insert(format.index);
        }
      }
    }

    // Cache 0-based indices of the XF records that refer to a
    // number format that is a date format
    int n_xfs = pWB->xfs.count;
    if (n_xfs == 0) {
      return;
    }

    for (int i = 0; i < n_xfs; ++i) {
      const xls::st_xf::st_xf_data xf = pWB->xfs.xf[i];
      int formatId = xf.format;
      if (isDateTime(formatId, customDateFormats)) {
        dateFormats_.insert(i);
      }
    }
  }

};

#endif
