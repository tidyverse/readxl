library(fs)
library(tidyverse)
library(here)
library(gert)
library(desc)

libxls_path <- "~/rrr/libxls"
there <- function(x) path(libxls_path, x)
target_branch <- "dev" # usually this is master, but not today

if (git_branch(repo = libxls_path) != target_branch) {
  message("YO! You are not on the target branch in libxls!")
}

# relevant when I am embedding an official release
#target_version <- "v1.6.2"
#(tag <- git_tag_list(target_version, repo = libxls_path))

libxls_SHA <- git_commit_id(repo = libxls_path)

# relevant when I am embedding an official release
# if (tag$commit != libxls_SHA) {
#    message("YO! SHA associated with HEAD isn't associated with target version!")
# }

# the subset of libxls files that we embed
paths <- c(
  "src/endian.c",
  "src/locale.c",
  "src/ole.c",
  "src/xls.c",
  "src/xlstool.c",
  "include/xls.h",
  "include/libxls/brdb.c.h",
  "include/libxls/brdb.h",
  "include/libxls/endian.h",
  "include/libxls/locale.h",
  "include/libxls/ole.h",
  "include/libxls/xlsstruct.h",
  "include/libxls/xlstool.h",
  "include/libxls/xlstypes.h"
)

file_copy(
  path     = there(paths),
  new_path = here(path("src", "libxls", path_file(paths))),
  overwrite = TRUE
)

# fixup for the case where I'm embedding a dev version
target_version <- "v1.6.2 (patched)"
desc::desc_set(Note = paste("libxls", target_version, substr(libxls_SHA, 1, 7)))

# as needed, I rerun the configure script to regenerate
# unix/config.h and windows/config.h

# as of libxls v1.6.2, we've had to adopt different static config files for
# macOS and other unix (basically motivated by what we see on GHA Ubuntu jobs)

# on windows, you may need to manually & temporarily add the directory
# containing gcc in Rtools to the PATH

# things I needed to do on a fresh Big Sur system to run ./bootstrap
# brew install autoconf
# brew install autoconf-archive
# brew install automake
# brew install gettext
# brew install libtool
# ./bootstrap
# ./configure

# I later learned from Jim that I could also download and unpack the libxls
# release and probably just run ./configure w/o installing so much stuff
