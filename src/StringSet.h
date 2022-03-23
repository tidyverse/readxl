#pragma once

#include "utils.h"

#include "cpp11/strings.hpp"

#include <cstring>
#include <set>
#include <vector>

class StringSet
{
  std::set<std::string> set_;
public:
  StringSet(const char *s = "") {
    if (std::strlen(s) > 0)
      set_.insert(s);
  }
  StringSet(const std::vector<std::string> &s) {
    for (auto i = s.begin(); i != s.end(); ++i)
      set_.insert(*i);
  }
  StringSet(const cpp11::strings &s) {
    for (auto i = s.begin(); i != s.end(); ++i)
      set_.insert(cpp11::as_cpp<std::string>(*i));
  }
  bool contains(const std::string &s) const {
    return set_.find(s) != set_.end();
  }
  bool contains(const std::string &s, const bool trimWs) const {
    return trimWs ? contains(trim(s)) : contains(s);
  }
  bool contains(const double d) const {
    std::ostringstream str; str << d;
    return contains(str.str());
  }
};
