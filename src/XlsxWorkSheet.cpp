#include "ColSpec.h"
#include "utils.h"

#include "cpp11/integers.hpp"
#include "cpp11/list.hpp"

#include <unistd.h>
#include <sys/time.h>

[[cpp11::register]]
cpp11::integers parse_ref(std::string ref) {
  std::pair<int,int> parsed = parseRef(ref.c_str());

  cpp11::writable::integers x = {parsed.first, parsed.second};
  return x;
}
