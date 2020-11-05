#### code to make batfiles ####

library(glue)
library(stringr)
library(tidyr)
library(dplyr)

landscape_size <- c(20L)
pop_density <- c(2, 5)
genmax <- c(100000L)
run_time <- c(10)
num_scenes <- c(10L)
f_cost <- c(0.5)
n_replicates <- 3

# make data
data_param <- crossing(
    rep_number = seq(n_replicates),
    landscape_size,
    pop_density,
    genmax,
    run_time,
    num_scenes,
    f_cost
)

# prepare lines
lines <- glue_data(
    data_param, 
    'IFD_activity \\
    {landscape_size} \\
    {pop_density} \\
    {genmax} \\
    {run_time} \\
    {num_scenes} \\
    {f_cost} \\
    {rep_number}')

# write to file
library(readr)
now <- str_replace_all(Sys.time(), ":", "_")
now <- str_replace_all(now, " ", "_time_")
path_to_bat <- glue("Release/runs_{now}.bat")

write_lines(lines,
            path = path_to_bat)