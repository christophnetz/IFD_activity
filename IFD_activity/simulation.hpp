#pragma once
#include "parameter.h"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <numeric>
#include <fstream>
#include <algorithm>
#include <string>

#include "rnd.hpp"

using namespace cine2;
using namespace std;




struct ind {

  ind() {}
  ind(int x, int y, double a) : xpos(x), ypos(y), act(a), food(0.0) {}



  void move(const vector<vector<double>>& landscape, vector<vector<int>>& presence);

  double food;
  double act;
  int xpos;
  int ypos;
};

void ind::move(const vector<vector<double>>& landscape, vector<vector<int>>& presence) {

  double present_intake = landscape[xpos][ypos] / static_cast<double> (presence[xpos][ypos]);
  double potential_intake;
  int former_xpos = xpos;
  int former_ypos = ypos;

  for (int i = 0; i < landscape.size(); ++i) {
    for (int j = 0; j < landscape[i].size(); ++j) {
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

bool check_IFD(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence) {


  for (int p = 0; p < pop.size(); ++p) {
    double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);

    for (int i = 0; i < landscape.size(); ++i) {
      for (int j = 0; j < landscape[i].size(); ++j) {
        if (present_intake < landscape[i][j] / (static_cast<double> (presence[i][j]) + 1.0)) {
          return false;
        }
      }
    }


  }
  return true;
}

double count_IFD(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence) {

  int count = pop.size();
  int p = 0;
label:
  for (; p < pop.size(); ++p) {
    double present_intake = landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);

    for (int i = 0; i < landscape.size(); ++i) {
      for (int j = 0; j < landscape[i].size(); ++j) {
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

double intake_variance(const vector<ind>& pop, const vector < vector<double>>& landscape, const vector<vector<int>>& presence) {
  vector<double> intakes;
  int p = 0;
  for (; p < pop.size(); ++p) {
    //intakes.push_back(landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]));
    intakes.push_back(landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]));
  }

  double sum = std::accumulate(intakes.begin(), intakes.end(), 0.0);
  double m = sum / intakes.size();

  double accum = 0.0;
  std::for_each(intakes.begin(), intakes.end(), [&](const double d) {
    accum += (d - m) * (d - m);
    });

  double stdev = sqrt(accum / (intakes.size()));

  return stdev;
}

void landscape_setup(vector<vector<double>>& landscape, const int popsize) {
  for (int i = 0; i < landscape.size(); ++i) {
    for (int j = 0; j < landscape[i].size(); ++j) {
      landscape[i][j] = uniform_real_distribution<double>(0.5 * popsize / 1000.0, 1.0 * popsize / 1000.0)(rnd::reng);
    }
  }
}

void reproduction(vector<ind>& pop, Param param_) {

  vector<double> fitness;

  for (int i = 0; i < pop.size(); ++i) {
    fitness.push_back(max(pop[i].food - param_.cost * pop[i].act * param_.t_scenes * param_.scenes, 0.0));
  }

  rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
  rdist.mutate(fitness.cbegin(), fitness.cend());

  vector<ind> tmp_pop;
  auto pdist = std::uniform_int_distribution<int>(0, param_.dims - 1);
  for (int i = 0; i < param_.pop_size; ++i) {
    tmp_pop.emplace_back(pdist(rnd::reng), pdist(rnd::reng), 0.5);
  }

  for (int i = 0; i < pop.size(); ++i) {
    const int ancestor = rdist(rnd::reng);
    tmp_pop[i].act = pop[ancestor].act;

    if (bernoulli_distribution(param_.mutation_rate)(rnd::reng)) {
      tmp_pop[i].act += normal_distribution<double>(0.0, param_.mutation_shape)(rnd::reng);
      tmp_pop[i].act = max(tmp_pop[i].act, 0.0);
    }
  }

  //using std::swap;
  swap(pop, tmp_pop);
}


void simulation(const Param& param_) {


  std::ofstream ofs1(param_.outdir + "activities.txt", std::ofstream::out);
  std::ofstream ofs2(param_.outdir + "ecology.txt", std::ofstream::out);

  ofs2 << "G" << "\t" << "ifd_prop" << "\t" << "time_to_IFD" << "\t" << "sd_intake" << "\t\n";


  std::ofstream os(param_.outdir + "params.txt", std::ofstream::out);
  stream_parameter(os, param_, "  ", ",\n", "c(", ")");
  os.close();

  //landscape initialization
  vector<vector<double>> landscape(param_.dims, vector<double>(param_.dims));
  for (int i = 0; i < landscape.size(); ++i) {
    for (int j = 0; j < landscape[i].size(); ++j) {
      landscape[i][j] = uniform_real_distribution<double>(param_.resource_min, param_.resource_max)(rnd::reng);
    }
  }


  vector<ind> pop;
  auto pdist = std::uniform_int_distribution<int>(0, param_.dims - 1);
  for (int i = 0; i < param_.pop_size; ++i) {
    pop.emplace_back(pdist(rnd::reng), pdist(rnd::reng), 0.5);
  }

  for (int g = 0; g < param_.G; ++g) {

    vector<double> activities;
    vector<vector<int>> presence(param_.dims, vector<int>(param_.dims, 0));
    for (int i = 0; i < pop.size(); ++i) {
      activities.push_back(pop[i].act);
      presence[pop[i].xpos][pop[i].ypos] += 1;
    }

    rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
    rdist.mutate(activities.cbegin(), activities.cend());
    double total_act = std::accumulate(activities.begin(), activities.end(), 0.0);
    exponential_distribution<double> event_dist(total_act);

    double ifd_prop = 0.0;
    double total_ttIFD = 0.0;
    double total_sdintake = 0.0;

    for (int scenes = 0; scenes < param_.scenes; ++scenes) {
      //cout << "scenes: " << scenes << endl;
      landscape_setup(landscape, param_.pop_size);

      double time = 0.0;
      int id;
      int eat_t = 0;
      double it_t = 0.0;
      double increment = 0.1;
      bool IFD_reached = false;
      double time_to_IFD = 0.0;

      for (; time < param_.t_scenes; ) {
        //cout << time << "\n";
        time += event_dist(rnd::reng);

        if (time > eat_t) {
          for (int p = 0; p < pop.size(); ++p) {
            pop[p].food += landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]);
          }
          ++eat_t;
        }


        if (!IFD_reached & time > it_t) {
          id = rdist(rnd::reng);
          pop[id].move(landscape, presence);
          IFD_reached = check_IFD(pop, landscape, presence);
          time_to_IFD = time;
          it_t = floor(time / increment) * increment + increment;
        }

      //cout << time << "\t" << IFD_reached << "\t" << endl;
      }
      //prop idf fulfilled
      ifd_prop += count_IFD(pop, landscape, presence);
      total_ttIFD += time_to_IFD;
      total_sdintake += intake_variance(pop, landscape, presence);
    }

    if (g % 10 == 0) {
      ofs1 << g << "\t";
      for (int q = 0; q < pop.size(); q += max(1, static_cast<int>(pop.size() / 1000))) {
        ofs1 << pop[q].act << "\t";
      }
      ofs1 << "\n";
      ofs2 << g << "\t" << ifd_prop / param_.scenes << "\t" << total_ttIFD / param_.scenes << total_sdintake / param_.scenes << "\t\n";
    }

    reproduction(pop, param_);
    cout << g << endl;
  }
  ofs1.close();
  ofs2.close();
  cout << "End";


}
