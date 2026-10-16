# Download the password-encrypted Excel fixtures from Excelize's test suite.
# Run from the package root:
#   Rscript tests/testthat/fixtures/download-excelize-encryption.R
#
# The fixtures decrypt with the password "password" to a single cell reading
# "SECRET". encryptAES.xlsx exercises ECMA-376 standard encryption;
# encryptSHA1.xlsx and encryptSHA512.xlsx exercise ECMA-376 agile encryption.
#
# Source: https://github.com/qax-os/excelize (BSD 3-Clause), pinned to a commit.

ref <- "5ecd16f78d68014f45b42f2a5ceae2023fc530cf"

fixtures <- c(
  encryptAES.xlsx = "7acdf072d4ba4af749220d6d7e0409cc",
  encryptSHA1.xlsx = "6279548509444a31b66acc6ae8b6796f",
  encryptSHA512.xlsx = "2b3755450db127b3bc0ba094aad8bf44"
)

dest_dir <- file.path("tests", "testthat", "sheets")

for (filename in names(fixtures)) {
  url <- paste0(
    "https://raw.githubusercontent.com/qax-os/excelize/",
    ref,
    "/test/",
    filename
  )
  target <- file.path(dest_dir, filename)
  download.file(url, target, mode = "wb", quiet = TRUE)

  actual_md5 <- unname(tools::md5sum(target))
  if (actual_md5 != fixtures[[filename]]) {
    stop("MD5 mismatch for ", filename, ": got ", actual_md5)
  }
}
