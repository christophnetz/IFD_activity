#!/bin/bash
#SBATCH --job-name=test
#SBATCH --time=01:00:00
#SBATCH --mem=500MB
#SBATCH --partition=regular
#SBATCH --output=test.log

pwd
module load foss/2018a
./myprog;
