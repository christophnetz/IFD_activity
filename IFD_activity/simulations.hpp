#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include "parameters.hpp"
#include "agents.hpp"
#include "landscape.hpp"
#include "rndutils.hpp"
#include "tools.hpp"

/// functions to check IFD
//bool check_IFD(const std::vector<ind>& pop, const std::vector < std::vector<double> >& landscape, const std::vector<std::vector<int> >& presence) {
//
//    for (size_t p = 0; p < pop.size(); ++p) {
//        double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
//
//        for (int i = 0; i < dims; ++i) {
//            for (int j = 0; j < dims; ++j) {
//                if (present_intake < landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0)) {
//                    return false;
//                }
//            }
//        }
//
//
//    }
//    return true;
//}

// function to check IFD
std::pair<bool, int> checkPCintake(const std::vector < std::vector<double> >& landscape,
    const std::vector<std::vector<int> >& presence)
{
    // vecs potential and realised intake
    std::vector<double> intakeReal (dims * dims), intakePot (dims * dims);

    size_t counter = 0;
    for (size_t i = 0; i < dims; ++i) {
        for (size_t j = 0; j < dims; ++j) {
            intakeReal[counter] = landscape[i][j] / static_cast<double>(presence[i][j]);
            intakePot[counter] = landscape[i][j] / (static_cast<double>(presence[i][j]) + 1.0);
            ++counter;
        }
    }
    // sort and compare
    std::sort(intakeReal.begin(), intakeReal.end());
    std::sort(intakePot.begin(), intakePot.end());

    // populate pair
    std::pair<bool, int> ifdMeasure;
    const double maxInPotential = intakePot[intakePot.size() - 1];
    ifdMeasure.first =  maxInPotential < intakeReal[0]; // if maxInPotential is greater than min intake real IFD is not reached; TRUE indicates success
    ifdMeasure.second = static_cast<int> (std::count_if(intakeReal.begin(), intakeReal.end(), [maxInPotential](double pcHere) {
        return pcHere < maxInPotential;
        }));
    return ifdMeasure;
}

//double count_IFD(const std::vector<ind>& pop, 
//    const std::vector < std::vector<double> >& landscape, 
//    const std::vector<std::vector<int> >& presence) {
//
//    size_t count = pop.size();
//    size_t p = 0;
//label:
//    for (; p < pop.size(); ++p) {
//        double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
//
//        for (int i = 0; i < dims; ++i) {
//            for (int j = 0; j < dims; ++j) {
//                if (present_intake < landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0)) {
//                    --count;
//                    ++p;
//                    goto label;
//                }
//            }
//        }
//
//
//    }
//    return static_cast<double>(count) / pop.size();
//}

// main fun to evolve pop
void evolvePop(
    const int g_start,
    const int genmax, const double run_time,
    const int num_scenes, const double f_cost,
    const std::string rep_number,
    std::vector<ind> & pop,
    std::vector<std::vector<double> > &landscape,
    std::vector<std::string> outputPath) 
{

    // tabulate time to ifd (genmax * 3 structure)
    std::vector<std::vector<double> > ifdTime (genmax);

    // begin looping over gens
    for (int g = g_start; g < genmax; ++g) {

        // get time to ifd
        std::vector<int> ttIfd(num_scenes);

        std::vector<double> activities(pop.size()); // activity levels of agents
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

                id = rdist(rng); // pick an agent to move
                pop[id].move(landscape, presence);
                
                // check ifd
                std::pair<bool, int> pcIntakeChecker = checkPCintake(landscape, presence);
                // add time to ifd vector
                if (pcIntakeChecker.first) {
                    ttIfd.push_back(time);
                }
            }
            //prop idf fulfilled
            //ifd_prop += count_IFD(pop, landscape, presence);
            //total_ttIFD += time_to_IFD;
        }
        std::vector<double> ttifdSummary(2);
        // get gen-wise mean and sd ttifd
        if (ttIfd.size() > 0) {
            ttifdSummary = meanSd(ttIfd);
        }
        else {
            ttifdSummary = { -99.0, -99.0 };
        }

        // print summary every 100 gens
        if (g % 100 == 0) {
            if (g % (genmax / 10) == 0) {
                std::cout << "gen = " << g << "\n";
            }
            printSummaryAct(pop, g, 0.001, outputPath);
            printTimeIfd(ttifdSummary, g, outputPath);
        }

        reproduction(pop, run_time, f_cost);
    }
}

// fun to subsample pop size to a different density
void reduceDensity(std::vector<ind> & pop, const int newDensity)
{
    const int newPopSize = dims * dims * newDensity;
    
    // make an int vector of ids and shuffle it
    std::vector<int> vecShuffle (pop.size());
    for (size_t ii = 0; ii < pop.size(); ++ii)
    {
        vecShuffle[ii] = static_cast<int>(ii);
    }
    std::shuffle(vecShuffle.begin(), vecShuffle.end(), rng);

    // select the first newPopSize agents
    std::vector<ind> newPop(newPopSize);
    for (size_t ii = 0; ii < static_cast<size_t> (newPopSize); ++ii)
    {
        newPop[ii] = pop[vecShuffle[ii]];
    }

    // swap pops
    assert(newPop.size() < pop.size());

    std::swap(pop, newPop);
    newPop.clear();
    std::cout << "pop density reduced\n";
}

void doSimulation(std::vector<std::string> cliArgs)
{
    // process arguments
    const int popDensity = std::stoi(cliArgs[1]);
    const int genmax = std::stoi(cliArgs[2]);
    const double run_time = std::stod(cliArgs[3]);
    const int num_scenes = std::stoi(cliArgs[4]);
    const double f_cost = std::stod(cliArgs[5]);
    const double newDensity = std::stoi(cliArgs[6]);
    const std::string rep_number = cliArgs[7];

    assert(newDensity < popDensity);
    
    // make landscape
	std::vector<std::vector<double> > landscape(dims, std::vector<double>(dims, 1.0));
	//landscape_setup(landscape);

    // make population flexible on density and landscape size
    // does not handle densities between 0 and 1 well
    const int popSize = dims * dims * popDensity;
    std::vector<ind> pop(popSize);

    // identify the output path and print to the lookup table
    unsigned seed =
        static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::clog << "random_seed : " << seed << '\n';
    rng.seed(seed);

    const std::vector<std::string> outputPath = identifyOutpath(
        popDensity, run_time, num_scenes, f_cost, rep_number, std::to_string(seed), newDensity
    );

    // run the population at original density
    evolvePop(0, genmax, run_time, num_scenes, f_cost, rep_number, pop, landscape, outputPath);

    reduceDensity(pop, newDensity);

    // run at new density
    evolvePop(genmax, genmax + genmax, run_time, num_scenes, f_cost, rep_number, pop, landscape, outputPath);

}
