library(glue)
library(stringr)
library(dplyr)
library(tidyr)
n_replicates <- 3

runs <- str_pad(seq_len(n_replicates), width = 3, pad = "0")
#G <- c(80000L)
#scenes <- c(10)
#t_scenes <- c(10)
pop_size <- c(50, 200, 1000, 10000)
#dims <- c(20)
#resource_min <- c(0.5)
#resource_max <- c(1.0)
#mutation_rate <- c(0.01)
#mutation_shape <- c(0.01)
#cost <- c(0.0001)

# make data
data_param <- crossing(replicate = runs, pop_size) %>% 
    mutate(outdir = glue('{pop_size}_{replicate}'),
           pop_size = pop_size)

data_param <- filter(data_param, pop_size == 50)
# prepare lines
lines <- glue_data(data_param, 'IFD_activity config=../settings/config.ini G=1000 pop_size={pop_size} outdir=../Results/sim{outdir}')

# write to file
library(readr)
write_lines(lines,
            file = "../IFD_activity/Release/runs_18_aug_fixed_2020.bat")


