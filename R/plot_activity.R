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
plots <- lapply(data, function(df) {
  ggplot(df)+
    geom_tile(aes(gen, actRound, fill = count))+
    scale_fill_viridis_c()
})
