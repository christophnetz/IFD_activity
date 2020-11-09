#### code to read and plot activity heatmap ####
library(data.table)
library(ggplot2)

# list files
data_files <- list.files("x64/Release/data/", pattern = "summary.csv",
                         full.names = T)

# load lookup table
lookup <- fread("x64/Release/data/lookup.csv")
lookup[, filename := as.character(filename)]

# read data and plot
data <- lapply(data_files, fread)

fileno <- stringi::stri_extract(data_files, regex = "\\d{4,}")

# match
data <- Map(function(df, fno) {
  df[, filename := fno]
}, data, fileno)

# add params
data <- Map(function(df) {
  df <- merge(df, lookup[, c("filename", 
                             "pop_density", "newDensity", "f_cost",
                             "rep_id")])
}, data)

# bind and split by combination
data <- rbindlist(data)

data <- split(data, by = c("pop_density", "newDensity"))

# count by cut
data_agg <- lapply(data, function(df){
  df_ <- copy(df)
  df_[, act_cut := cut(actRound, breaks = seq(0, 1, 0.005), include.lowest = T)]
  df_ <- df_[, list(count = .N), by = c("pop_density", "newDensity", "f_cost", 
                          "rep_id", "act_cut", "gen")]
})

# set dims and gen change
dims <- 20
gen_change <- 10000

# get proportions recalling counts change with density
data_agg <- lapply(data_agg, function(df) {
  df[, prop := count / (fifelse(gen <= gen_change, 
                                unique(dims * dims * pop_density),
                                unique(dims * dims * newDensity)))]
})

# make plots
plots <- Map(function(df) {
  ggplot(df[inrange(gen, 100, 15000), ])+
    geom_vline(xintercept = 10000, size = 0.1, col = "black")+
    geom_tile(aes(gen, act_cut, fill = prop),
              show.legend = F)+
    scale_fill_viridis_c(option = "C", direction = -1)+
    facet_grid(f_cost ~ rep_id, labeller = label_both)+
    scale_x_continuous(labels = scales::comma_format(scale = 0.001, 
                                                     suffix = "K"))+
    theme_minimal()+
    theme(axis.text.y = element_blank(),
          panel.grid = element_blank())+
    labs(title = glue::glue('density = {unique(df$pop_density)} \\
                            new density = {unique(df$newDensity)}'))
}, data_agg)

# order by name
plots <- plots[order(names(plots))]

plots[[10]]
patchwork::wrap_plots(plots)

ggsave(filename = "figures/fig_activity_heatmap.png")
