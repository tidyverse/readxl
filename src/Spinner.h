#ifndef READXL_SPINNER_
#define READXL_SPINNER_

#include <RProgress.h>

class Spinner {
  RProgress::RProgress pb_;

public:
  Spinner(std::string format = ":spin") {
    pb_ = RProgress::RProgress(format);
    pb_.set_total(1);
    pb_.set_show_after(2);
  }
  void spin() { pb_.update(0.5); }
  void finish() { pb_.update(1); }
};

#endif
