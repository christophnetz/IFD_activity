#### code to make batfiles ####

library(glue)
library(stringr)
library(tidyr)
library(dplyr)

pop_density <- c(2, 5)
new_density <- c(1, 2)
genmax <- c(50000L)
run_time <- c(10)
num_scenes <- c(10L)
f_cost <- c(0.5)
n_replicates <- 3

# make data
data_param <- crossing(
    rep_number = seq(n_replicates),
    pop_density,
    new_density,
    genmax,
    run_time,
    num_scenes,
    f_cost
)

# filter
data_param <- filter(data_param, pop_density > new_density)

# prepare lines
lines <- glue_data(
    data_param, 
    'IFD_activity \\
    {pop_density} \\
    {genmax} \\
    {run_time} \\
    {num_scenes} \\
    {f_cost} \\
    {new_density} \\
    {rep_number}')

# write to file
library(readr)
now <- str_replace_all(Sys.time(), ":", "_")
now <- str_replace_all(now, " ", "_time_")
path_to_bat <- glue("x64/Release/runs_{now}.bat")

write_lines(lines,
            path = path_to_bat)
