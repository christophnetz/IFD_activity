#pragma once
#include <vector>
#include <random>
#include "parameters.hpp"

// function to setup landscape
// dim is hardcoded in parameters.hpp
void landscape_setup(std::vector<std::vector<double>>& landscape) {
    for (int i = 0; i < dims; ++i) {
        for (int j = 0; j < dims; ++j) {
            landscape[i][j] = std::uniform_real_distribution<double>(0.0, 1.0)(rng);
        }
    }
}
