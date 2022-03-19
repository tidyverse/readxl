devtools::clean_dll()
devtools::load_all()

files <- fs::dir_ls("investigations/Data", recurse = TRUE, glob = "*.XLS")
#files
read_xls(files[[1]])

