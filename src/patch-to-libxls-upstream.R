library(fs)
library(tidyverse)
library(here)
library(gert)
library(desc)

libxls_path <- "~/rrr/libxls"
libxls_SHA <- git_commit_id(repo = libxls_path)
there <- function(x) path(libxls_path, x)

if (git_branch(repo = libxls_path) != "master") {
  message("YO! You are not on master in libxls! Are you sure about this?")
}

## the subset of libxls files that we embed
paths <- c(
  "src/endian.c",
  "src/ole.c",
  "src/xls.c",
  "src/xlstool.c",
  "include/xls.h",
  "include/libxls/brdb.c.h",
  "include/libxls/brdb.h",
  "include/libxls/endian.h",
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

desc::desc_set(Note = paste("libxls-SHA", substr(libxls_SHA, 1, 7)))

## at this point, you'll have a diff
## selectively commit the bits we truly want; call this commit X
## now commit the reversals of our usual patches; call this commit Y
## revert commit Y; this ADDS our usual patches; call this commit Z
## rebase and squash X and Y
## rebase and edit the message for commit Z
## revel in all the xls issues that are newly resolved

## as needed (less often), I rerun the configure script to regenerate
## unix/config.h and windows/config.h
## on windows, you may need to manually & temporarily add the directory
## containing gcc in Rtools to the PATH
## we have manually applied patches in config.h as well
## basically some fixes we have long had around iconv prototypes have moved out
## of xlstool.c and into config.h, which is probably a good thing
