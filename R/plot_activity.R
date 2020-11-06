#### code to read and plot activity heatmap ####
library(data.table)
library(ggplot2)

# list files
data_files <- list.files("x64/Release/data/", pattern = "summary.csv",
                         full.names = T)

# load lookup table
lookup <- fread("x64/Release/data/lookup.csv")

# read data and plot
data <- lapply(data_files, fread)

# make plots
plots <- Map(function(df, d1, d2) {
  ggplot(df)+
    geom_tile(aes(gen, actRound, fill = count),
              show.legend = F)+
    scale_fill_viridis_c(option = "C", direction = -1,
                         limits = c(0, 200),
                         na.value = "blue")+
    theme_void()+
    labs(title = glue::glue('density = {d1}, new density = {d2}'))
}, data, lookup$pop_density, lookup$newDensity)

patchwork::wrap_plots(plots)
