test_that("color extraction works correctly", {
  file <- readxl_example("gapminder-2007.xlsx")
  
  # Read the "good" sheet without color extraction
  good_data <- read_excel(file, sheet = "good")
  continents <- unique(good_data$continent)
  n_continents <- length(continents)
  
  # Read the "bad" sheet with color extraction
  bad_data <- read_excel(file, sheet = "bad", extract_colors = TRUE)
  
  # Get unique bg colors (excluding NAs)
  bg_colors <- unique(bad_data$country_bg)
  bg_colors <- bg_colors[!is.na(bg_colors)]
  
  # The number of unique colors should match the number of continents
  expect_equal(length(bg_colors), n_continents)

  # Conditional formatting rules from XLSX (similar test as above)
  mean_categories <- unique(good_data$lifeExpOverContinentAvg)
  mean_colors <- unique(bad_data$lifeExp_bg)
  expect_equal(length(mean_colors), length(mean_categories))
})
