#pragma once

#include <vector>
#include <string>
#include "parameters.hpp"
#include "agents.hpp"
#include "landscape.hpp"
#include "rndutils.hpp"
#include "tools.hpp"

/// functions to check IFD
bool check_IFD(const std::vector<ind>& pop, const std::vector < std::vector<double>>& landscape, const std::vector<std::vector<int>>& presence) {

    for (size_t p = 0; p < pop.size(); ++p) {
        double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);

        for (int i = 0; i < dims; ++i) {
            for (int j = 0; j < dims; ++j) {
                if (present_intake < landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0)) {
                    return false;
                }
            }
        }


    }
    return true;
}

double count_IFD(const std::vector<ind>& pop, const std::vector < std::vector<double>>& landscape, const std::vector<std::vector<int>>& presence) {

    size_t count = pop.size();
    size_t p = 0;
label:
    for (; p < pop.size(); ++p) {
        double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);

        for (int i = 0; i < dims; ++i) {
            for (int j = 0; j < dims; ++j) {
                if (present_intake < landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0)) {
                    --count;
                    ++p;
                    goto label;
                }
            }
        }


    }
    return static_cast<double>(count) / pop.size();
}

void doSimulation(std::vector<std::string> cliArgs)
{
    // process arguments
    const int popDensity = std::stoi(cliArgs[1]);
    const int genmax = std::stoi(cliArgs[2]);
    const double run_time = std::stod(cliArgs[3]);
    const int num_scenes = std::stoi(cliArgs[4]);
    const double f_cost = std::stod(cliArgs[5]);
    const std::string rep_number = cliArgs[6];
    
    // make landscape
	std::vector<std::vector<double> > landscape(dims, std::vector<double>(dims, 1.0));
	//landscape_setup(landscape);

    // make population flexible on density and landscape size
    // does not handle densities between 0 and 1 well
    const int popSize = dims * popDensity;
    std::vector<ind> pop(popSize);

    // identify the output path and print to the lookup table
    unsigned seed =
        static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::clog << "random_seed : " << seed << '\n';
    rng.seed(seed);

    const std::vector<std::string> outputPath = identifyOutpath(
        popDensity, run_time, num_scenes, f_cost, rep_number, std::to_string(seed)
    );

    // begin looping over gens
    for (int g = 0; g < genmax; ++g) {

        std::vector<double> activities (popSize); // activity levels of agents
        std::vector<std::vector<int> > presence(dims, std::vector<int>(dims, 0)); // agent counts

        // populate activity vec and presence grid
        for (size_t i = 0; i < pop.size(); ++i) {
            activities[i] = pop[i].act;
            presence[static_cast<size_t> (pop[i].xpos)][static_cast<size_t>(pop[i].ypos)] += 1;
        }

        // distribution of mutations? unclear
        rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
        rdist.mutate(activities.cbegin(), activities.cend()); // add some mutations? unclear
        
        // sum activity levels and make exponential distr
        double total_act = std::accumulate(activities.begin(), activities.end(), 0.0);
        std::exponential_distribution<double> event_dist(total_act);

        double ifd_prop = 0.0; // p pop in ifd
        double total_ttIFD = 0.0; // time to ifd

        // run over some N scenes, default 10
        for (int scenes = 0; scenes < num_scenes; ++scenes) {
            
            // re-init landscape
            landscape_setup(landscape);

            double time = 0.0;
            int id;
            int eat_t = 0;
            bool IFD_reached = false;
            double time_to_IFD = 0.0;

            // gillespie loop here?
            for (; time < run_time; ) {
                //cout << time << "\n";
                time += event_dist(rng);

                if (time > eat_t) {
                    for (int p = 0; p < pop.size(); ++p) {
                        pop[p].food += landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
                    }
                    ++eat_t;
                }

                //if (!IFD_reached) {
                id = rdist(rng); // pick an agent to move
                pop[id].move(landscape, presence);
                //IFD_reached = check_IFD(pop, landscape, presence);
                //time_to_IFD = time;
              //}

              //cout << time << "\t" << IFD_reached << "\t" << endl;
            }
            //prop idf fulfilled
            //ifd_prop += count_IFD(pop, landscape, presence);
            //total_ttIFD += time_to_IFD;
        }

        // print summary every 100 gens
        if (g % 100 == 0) {
            if (g % (genmax / 10) == 0) {
                std::cout << "gen = " << g << "\n";
            }
            printSummaryMass(pop, g, 0.001, outputPath);
        }

        reproduction(pop, run_time, f_cost);
    }

}
