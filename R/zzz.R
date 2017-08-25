# from https://github.com/jimhester/readxl/blob/87b4d03fcb1952ef75cc5359cf2e0bf936a7abbd/R/example.R
#' @importFrom completeme return_unless current_function inside_quotes
readxl_example_completer <- function(env) {
  return_unless(current_function(env) == "readxl_example" && inside_quotes(env))

  # Completion within readxl_example uses the example filenames
  grep(paste0("^\"?", env$token, "."), readxl_example(), value = TRUE)
}

.onLoad <- function(pkg, lib) {
  completeme::register_completion(readxl = readxl_example_completer)
}
