#pragma once

/// Parameters
/// changed to cli args
const int dims = 20; // the landscape size in gridcells^2
//const int pop_size = 1000;
//const int Gmax = 100000;
//const int run_time = 10;//100
double mutation_rate = 0.01; //0.001
double mutation_shape = 0.05;//0.1
//const int num_scenes = 10;//10
//const double fcost = 0.5;
//const string ID_run = "01_3-11";

std::mt19937_64 rng;
