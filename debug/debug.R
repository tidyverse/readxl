devtools::clean_dll()
devtools::load_all()

# success <- read_excel(
#   "investigations/sample_data/success.xls",
#   col_names = FALSE,
#   range = "A1"
# )
failure <- read_excel("investigations/sample_data/failure.xls", col_names=F)

# cell_is_readable
# cell->id is 214 for the failure
# 214 in hexadecimal is 0xD6
# in xlsstruct.h:
# #define XLS_RECORD_RSTRING      0x00D6

# cell->id is 516 for the success
# 516 in hexadecimal is 0x204
# in xlsstruct.h:
# #define XLS_RECORD_LABEL        0x0204
