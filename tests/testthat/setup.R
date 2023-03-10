withr::local_options(
  list(rlib_name_repair_verbosity = "quiet"),
  .local_envir = teardown_env()
)
