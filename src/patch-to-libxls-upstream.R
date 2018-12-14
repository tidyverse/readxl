library(fs)
library(tidyverse)
library(here)
library(git2r)
library(desc)

libxls_path <- "~/rrr/libxls-evanmiller-github"
libxls_SHA <- sha(last_commit(libxls_path))
there <- function(x) path(libxls_path, x)

if (repository_head(libxls_path)[["name"]] != "master") {
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

new_paths <- paths

## btw xls.h doesn't live with the other headers in libxls
## but it needs to do so in readxl
header_file <- grepl(".h$", new_paths)
new_paths[header_file] <-
  path("src", "libxls", path_file(new_paths))[header_file]

file_copy(path = there(paths), new_path = here(new_paths), overwrite = TRUE)

desc::desc_set(Note = paste("libxls-SHA", substr(libxls_SHA, 1, 7)))

## at this point, you'll have a diff
## selectively commit the bits we truly want; call this commit X
## now commit the reversals of our usual patches; call this commit Y
## revert commit Y; this ADDS our usual patches; call this commit Z
## rebase and squash X and Y
## rebase and edit the message for commit Z
## revel in all the xls issues that are newly resolved
