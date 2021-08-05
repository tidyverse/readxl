#ifndef READXL_ZIP_
#define READXL_ZIP_

#include "rapidxml.h"
#include <string>

std::string zip_buffer(const std::string& zip_path, const std::string& file_path);
bool zip_has_file(const std::string& zip_path, const std::string& file_path);

#endif
