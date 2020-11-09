#pragma once

#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>
#include "parameters.hpp"

struct ind {

    ind() {
        food = 0.0;
        act = std::uniform_real_distribution<double>(0.2, 0.8)(rng);
        xpos = std::uniform_int_distribution<int>(0, dims - 1)(rng);
        ypos = std::uniform_int_distribution<int>(0, dims - 1)(rng);

    }

    void move(const std::vector<std::vector<double>>& landscape, std::vector<std::vector<int>>& presence);

    double food;
    double act;
    int xpos;
    int ypos;
};

void ind::move(const std::vector<std::vector<double>>& landscape, std::vector<std::vector<int>>& presence) {

    double present_intake = landscape[xpos][ypos] / static_cast<double> (presence[xpos][ypos]);
    double potential_intake;
    int former_xpos = xpos;
    int former_ypos = ypos;

    for (int i = 0; i < dims; ++i) {
        for (int j = 0; j < dims; ++j) {
            potential_intake = landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0);
            if (present_intake < potential_intake) {
                present_intake = potential_intake;
                xpos = i;
                ypos = j;
            }
        }
    }

    presence[xpos][ypos] += 1;
    presence[former_xpos][former_ypos] -= 1;
}

// reproduction function
void reproduction(std::vector<ind>& pop, const double run_time, const double fcost ) {

    std::vector<double> fitness;

    for (int i = 0; i < pop.size(); ++i) {
        fitness.push_back(std::max(pop[i].food - (fcost * pop[i].act * run_time), 0.0));
    }

    rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
    rdist.mutate(fitness.cbegin(), fitness.cend());
    auto tmp_pop = pop;

    for (int i = 0; i < pop.size(); ++i) {
        const int ancestor = rdist(rng);
        tmp_pop[i] = pop[ancestor];
        tmp_pop[i].xpos = std::uniform_int_distribution<int>(0, dims - 1)(rng);
        tmp_pop[i].ypos = std::uniform_int_distribution<int>(0, dims - 1)(rng);
        tmp_pop[i].food = 0.0;

        if (std::bernoulli_distribution(mutation_rate)(rng)) {
            tmp_pop[i].act += std::normal_distribution<double>(0.0, mutation_shape)(rng);
            tmp_pop[i].act = std::max(tmp_pop[i].act, 0.0);
        }
    }

    //using std::swap;
    swap(pop, tmp_pop);
    tmp_pop.clear();
}
