#### code to plot time to ifd ####
library(data.table)
library(ggplot2)

# list files
ttifd_files <- list.files("x64/Release/data/", pattern = "ttifd.csv",
                         full.names = T)

# load lookup table
lookup <- fread("x64/Release/data/lookup.csv")

# read data and plot
data_ttifd <- lapply(ttifd_files, fread, na.strings = "-nan(ind)")

# make plots
plots <- lapply(data_ttifd, function(df) {
  ggplot(df)+
    geom_point(aes(gen, ttifd_mean))+
    geom_errorbar(aes(gen, 
                      ymin = ttifd_mean - ttifd_sd,
                      ymax = ttifd_mean + ttifd_sd),
                  width = 0)
})

plots[[3]]
