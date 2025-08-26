test_that("color extraction works correctly", {
  file <- readxl_example("gapminder-2007.xlsx")
  
  # Read the "good" sheet without color extraction
  good_data <- read_excel(file, sheet = "good")
  continents <- unique(good_data$continent)
  n_continents <- length(continents)
  
  # Read the "bad" sheet with color extraction
  bad_data <- read_excel(file, sheet = "bad", extract_colors = TRUE)
  
  # Get unique colors (excluding NAs)
  colors <- unique(bad_data$country_bg)
  colors <- colors[!is.na(colors)]
  n_colors <- length(colors)
  
  # The number of unique colors should match the number of continents
  expect_equal(n_colors, n_continents)
})
