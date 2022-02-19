#pragma once

#include <RProgress.h>

const int PROGRESS_TICK = 131072; // 2^17

class Spinner {
  bool progress_;
  RProgress::RProgress pb_;

public:
  Spinner(bool progress = true):
  progress_(progress)
  {
    if (progress_) {
      pb_ = RProgress::RProgress(":spin");
      pb_.set_total(1);
      pb_.set_show_after(2);
    }
  }
  void spin()   { if (progress_) pb_.update(0.5); }
  void finish() { if (progress_) pb_.update(1);   }
  ~Spinner() { if (this->progress_) this->finish(); }
};
