# from https://github.com/jimhester/readxl/blob/87b4d03fcb1952ef75cc5359cf2e0bf936a7abbd/R/example.R
readxl_example_completer <- function(env) {
  if (completeme::current_function(env) == "readxl_example" && completeme::inside_quotes(env)) {

    # Completion within readxl_example uses the example filenames
    comps <- grep(paste0("^\"?", env$token, "."), readxl_example(), value = TRUE)
    env[["fileName"]] <- comps
    env[["comps"]] <- comps
    return (TRUE)
  }
  return (FALSE)
}

.onLoad <- function(pkg, lib) {
  completeme::register_completion(readxl = readxl_example_completer)
}
