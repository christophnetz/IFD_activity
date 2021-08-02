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




struct cell {
  cell() {}
  cell(double resource, double risk) : resource(resource), risk(risk) {}

  double resource;
  double risk;
};

struct ind {

  ind() {}
  ind(int x, int y, double a, double c, double b) : xpos(x), ypos(y), act(a), comp(c), bold(b) {}

  void die(double& presence);
  void mutate(bernoulli_distribution& mutate, normal_distribution<double>& mshape);
  void move(const vector<vector<cell>>& landscape, vector<vector<double>>& presence);
  void springoff(const ind& parent);

  bool dead = false;
  double food = 0.0;
  double act;
  double comp;
  double bold;
  int xpos;
  int ypos;
};


void ind::move(const vector<vector<cell>>& landscape, vector<vector<double>>& presence) {

  if (!dead) {
    double present_intake = landscape[xpos][ypos].resource * comp / (presence[xpos][ypos]) /*+ bold * landscape[xpos][ypos].risk*/;
    double potential_intake;
    int former_xpos = xpos;
    int former_ypos = ypos;

    //std::uniform_real_distribution<double>error (0.0, 1.0);

    for (int i = 0; i < landscape.size(); ++i) {
      for (int j = 0; j < landscape[i].size(); ++j) {
        potential_intake = landscape[i][j].resource * comp / (presence[i][j] + comp);
        if (present_intake < potential_intake) {
          present_intake = potential_intake;
          xpos = i;
          ypos = j;
        }
      }
    }

    presence[xpos][ypos] += comp;
    presence[former_xpos][former_ypos] -= comp;
  }
}

void ind::die(double& presence) {
    food = 0.0;
    presence -= comp;
    comp = 0.0;
    dead = true;
}

void ind::mutate(bernoulli_distribution& mrate, normal_distribution<double>& mshape) {

  if (mrate(rnd::reng)) {
    act += mshape(rnd::reng);
    act = max(act, 0.0);
  }

  if (mrate(rnd::reng)) {
    bold += mshape(rnd::reng); 
    bold = max(bold, 0.0);
    bold = min(bold, 1.0);
  }

  if (mrate(rnd::reng)) {
    comp += mshape(rnd::reng);
    comp = max(comp, 0.1);  // Can't be 0, better way to implement?
  }
}

void ind::springoff(const ind& parent) {
  act = parent.act;
  comp = parent.comp;
  bold = parent.bold;
}


bool check_IFD(const vector<ind>& pop, const vector < vector<cell>>& landscape, const vector<vector<double>>& presence) {

     
  for (int p = 0; p < pop.size(); ++p) {
    double present_intake = landscape[pop[p].xpos][pop[p].ypos].resource * pop[p].comp / presence[pop[p].xpos][pop[p].ypos];

    for (int i = 0; i < landscape.size(); ++i) {
      for (int j = 0; j < landscape[i].size(); ++j) {
        if (present_intake < landscape[i][j].resource * pop[p].comp / (presence[i][j] + pop[p].comp)) {
          return false;
        }
      }
    }


  }
  return true;
}

double count_IFD(const vector<ind>& pop, const vector < vector<cell>>& landscape, const vector<vector<double>>& presence) {

  int count = pop.size();
  int p = 0;
label:
  for (; p < pop.size(); ++p) {
    double present_intake = landscape[pop[p].xpos][pop[p].ypos].resource * pop[p].comp / presence[pop[p].xpos][pop[p].ypos];

    for (int i = 0; i < landscape.size(); ++i) {
      for (int j = 0; j < landscape[i].size(); ++j) {
        if (present_intake < landscape[i][j].resource * pop[p].comp / (presence[i][j] + pop[p].comp)) {
          --count;
          ++p;
          goto label;
        }
      }
    }


  }
  return static_cast<double>(count) / pop.size();
}

double intake_variance(const vector<ind>& pop, const vector < vector<cell>>& landscape, const vector<vector<double>>& presence) {
  vector<double> intakes;
  int p = 0;
  for (; p < pop.size(); ++p) {
    //intakes.push_back(landscape[pop[p].xpos][pop[p].ypos] / static_cast<double> (presence[pop[p].xpos][pop[p].ypos]));
    intakes.push_back(landscape[pop[p].xpos][pop[p].ypos].resource * pop[p].comp / presence[pop[p].xpos][pop[p].ypos]);
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

void landscape_setup(vector<vector<cell>>& landscape, Param param_) {
  double cells = static_cast<double>(landscape.size() * landscape.size());
  for (int i = 0; i < landscape.size(); ++i) {
    for (int j = 0; j < landscape[i].size(); ++j) {
      //landscape[i][j] = cell(uniform_real_distribution<double>(param_.resource_min * param_.pop_size * 400.0 / (1000.0 * cells), param_.resource_max * param_.pop_size * 400 / (1000.0 * cells))(rnd::reng), 0.0);
      landscape[i][j] = cell(uniform_real_distribution<double>(param_.resource_min, param_.resource_max)(rnd::reng),
        /*exponential_distribution<double>(param_.riskspread)(rnd::reng)*/0.0);
    }
  }
}

void reproduction(vector<ind>& pop, const Param& param_) {

  vector<double> fitness;

  for (int i = 0; i < pop.size(); ++i) {
    fitness.push_back(max(0.0, pop[i].food - param_.cost * pop[i].act * param_.t_scenes * param_.scenes - param_.cost_comp * pop[i].comp * param_.t_scenes * param_.scenes));
  }

  rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
  rdist.mutate(fitness.cbegin(), fitness.cend());

  vector<ind> tmp_pop(pop.size());
  uniform_int_distribution<int> pdist(0, param_.dims - 1);
  bernoulli_distribution mrate(param_.mutation_rate);
  normal_distribution<double> mshape(0.0, param_.mutation_shape);

  for (int i = 0; i < pop.size(); ++i) {
    const int ancestor = rdist(rnd::reng);
    tmp_pop[i].springoff(pop[ancestor]);
    tmp_pop[i].xpos = pdist(rnd::reng);
    tmp_pop[i].ypos = pdist(rnd::reng);

    tmp_pop[i].mutate(mrate, mshape);
  }

  //using std::swap;
  swap(pop, tmp_pop);
}


void simulation(const Param& param_) {

  if(param_.seed != 0)
    rnd::reng.seed(param_.seed);
  


  std::ofstream ofs1(param_.outdir + "activities.txt", std::ofstream::out);
  std::ofstream ofs2(param_.outdir + "ecology.txt", std::ofstream::out);
  std::ofstream ofs3(param_.outdir + "comp.txt", std::ofstream::out);
  std::ofstream ofs4(param_.outdir + "bold.txt", std::ofstream::out);

  ofs2 << "G" << "\t" << "ifd_prop" << "\t" << "time_to_IFD" << "\t" << "sd_intake" << "\t\n";


  std::ofstream os(param_.outdir + "params.txt", std::ofstream::out);
  stream_parameter(os, param_, "  ", ",\n", "c(", ")");
  os.close();

  //landscape initialization
  vector<vector<cell>> landscape(param_.dims, vector<cell>(param_.dims));



  vector<ind> pop;
  auto pdist = std::uniform_int_distribution<int>(0, param_.dims - 1);
  for (int i = 0; i < param_.pop_size; ++i) {
    pop.emplace_back(pdist(rnd::reng), pdist(rnd::reng), 0.5, 1.0, 0.0);
  }

  for (int g = 0; g < param_.G; ++g) {

    

    vector<double> activities;
    vector<vector<double>> presence(param_.dims, vector<double>(param_.dims, 0.0));
    for (int i = 0; i < pop.size(); ++i) {
      activities.push_back(pop[i].act);
      presence[pop[i].xpos][pop[i].ypos] += pop[i].comp;
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
      landscape_setup(landscape, param_);
      // Randomly initialize individuals:
      //for (int i = 0; i < param_.pop_size; ++i) {
      //  pop[i].xpos = pdist(rnd::reng);
      //  pop[i].ypos = pdist(rnd::reng);
      //}


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

        while (time > eat_t) { // alternative: individuals eat continuously. Maybe let's not
          for (int p = 0; p < pop.size(); ++p) {
            if (!pop[p].dead) {
              pop[p].food += landscape[pop[p].xpos][pop[p].ypos].resource * pop[p].comp / presence[pop[p].xpos][pop[p].ypos];
              //if(bernoulli_distribution(landscape[pop[p].xpos][pop[p].ypos].risk)(rnd::reng)) 
              //  pop[p].die(presence[pop[p].xpos][pop[p].ypos]);
            }
          }
          ++eat_t;
        }



        if (!IFD_reached) {
          id = rdist(rnd::reng);
          pop[id].move(landscape, presence);
          if (time > it_t) {
            IFD_reached = check_IFD(pop, landscape, presence);
            time_to_IFD = time;
            it_t = floor(time / increment) * increment + increment;
          }

        }
        //cout << time << "\t" << IFD_reached << "\t" << endl;
      }
      //prop idf fulfilled
      ifd_prop += count_IFD(pop, landscape, presence);
      total_ttIFD += time_to_IFD;
      total_sdintake += intake_variance(pop, landscape, presence);  //Correct intake for competitiveness?
    }

    if (g % 10 == 0) {
      ofs1 << g << "\t";
      ofs3 << g << "\t";
      ofs4 << g << "\t";

      for (int q = 0; q < pop.size(); q += max(1, static_cast<int>(pop.size() / 1000))) {
        ofs1 << pop[q].act << "\t";
        ofs3 << pop[q].comp << "\t";
        ofs4 << pop[q].bold << "\t";
      }
      ofs1 << "\n";
      ofs3 << "\n";
      ofs4 << "\n";
      ofs2 << g << "\t" << ifd_prop / param_.scenes << "\t" << total_ttIFD / param_.scenes << total_sdintake / param_.scenes << "\t\n";
    }

    reproduction(pop, param_);
    cout << g  << "\t" << pdist(rnd::reng) << endl;
  }
  ofs1.close();
  ofs2.close();
  cout << "End";

  

}
