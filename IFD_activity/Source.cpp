#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <numeric>
#include <fstream>
#include <algorithm>
#include <string>
#include "rndutils.hpp"
#include "parameters.hpp"
#include "agents.hpp"
#include "simulations.hpp"

//using namespace std;


int main(int argc, char* argv[]) {

    // collect args
    std::vector<std::string> cliArgs(argv, argv + argc);

  /*unsigned seed =
    static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::clog << "random_seed : " << seed << '\n';
  rng.seed(seed);*/

  doSimulation(cliArgs);

  //std::string idRun = std::to_string(seed);

  //// take ID_run from seed which is time -- allows precise date-time recovery from file name
  //std::ofstream ofs1(idRun + "_activities.txt", std::ofstream::out);
  ////std::ofstream ofs2("IDF2.txt", std::ofstream::out);
  ////ofs2 << "G" << "\t" << "prop_ifd" << "\t" << "avg_ttifd" << "\t" << endl;

  //std::ofstream ofs3(idRun + "params.txt", std::ofstream::out);
  //ofs3 << "dims" << "\t" << dims << "\n"
  //  << "pop_size" << "\t" << pop_size << "\n"
  //  << "Gmax" << "\t" << Gmax << "\n"
  //  << "run_time" << "\t" << run_time << "\n"
  //  << "num_scenes" << "\t" << num_scenes << "\n"
  //  << "mutation_rate" << "\t" << mutation_rate << "\n"
  //  << "mutation_shape" << "\t" << mutation_shape << "\n"
  //  << "fcost" << "\t" << fcost << "\n";

  //ofs3.close();


  //landscape initialization
  //std::vector<std::vector<double>> landscape(dims, std::vector<double>(dims, 1.0));
  //landscape_setup(landscape);

  //std::vector<ind> pop(pop_size);

  //for (int g = 0; g < Gmax; ++g) {

  //  std::vector<double> activities;
  //  std::vector<std::vector<int>> presence(dims, std::vector<int>(dims, 0));
  //  for (int i = 0; i < pop.size(); ++i) {
  //    activities.push_back(pop[i].act);
  //    presence[pop[i].xpos][pop[i].ypos] += 1;
  //  }

  //  rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
  //  rdist.mutate(activities.cbegin(), activities.cend());
  //  double total_act = std::accumulate(activities.begin(), activities.end(), 0.0);
  //  exponential_distribution<double> event_dist(total_act);

  //  double ifd_prop = 0.0;
  //  double total_ttIFD = 0.0;

  //  for (int scenes = 0; scenes < num_scenes; ++scenes) {
  //    //cout << "scenes: " << scenes << endl;
  //    landscape_setup(landscape);

  //    double time = 0.0;
  //    int id;
  //    int eat_t = 0;
  //    bool IFD_reached = false;
  //    double time_to_IFD = 0.0;

  //    for (; time < run_time; ) {
  //      //cout << time << "\n";
  //      time += event_dist(rng);

  //      if (time > eat_t) {
  //        for (int p = 0; p < pop.size(); ++p) {
  //          pop[p].food += landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
  //        }
  //        ++eat_t;
  //      }


  //      //if (!IFD_reached) {
  //      id = rdist(rng);
  //      pop[id].move(landscape, presence);
  //      //IFD_reached = check_IFD(pop, landscape, presence);
  //      //time_to_IFD = time;
  //    //}

  //    //cout << time << "\t" << IFD_reached << "\t" << endl;
  //    }
  //    //prop idf fulfilled
  //    //ifd_prop += count_IFD(pop, landscape, presence);
  //    //total_ttIFD += time_to_IFD;
  //  }

  //  if (g % 10 == 0) {
  //    ofs1 << g << "\t";
  //    for (int q = 0; q < pop.size(); ++q) {
  //      ofs1 << pop[q].act << "\t";
  //    }
  //    ofs1 << "\n";
  //    //ofs2 << g << "\t" << ifd_prop / num_scenes << "\t" << total_ttIFD / num_scenes << "\t\n";
  //  }

  //  reproduction(pop);
  //  cout << g << endl;
  //}
  //ofs1.close();
  ////ofs2.close();
  //cout << "End";

  return 0;
}