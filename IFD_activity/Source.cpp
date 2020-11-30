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

using namespace std;
/// Parameters
const int dims = 20;
const int pop_size = 10000;
const int Gmax = 80000;
const int run_time = 10;//100
double mutation_rate = 0.01; //0.001
double mutation_shape = 0.0100;//0.1
const int num_scenes = 10;//10
const double fcost = 0.0001;
const string ID_run = "03_29-11";

std::mt19937_64 rng;

struct ind {

  ind() {
    food = 0.0;
    act = 0.5;// uniform_real_distribution<double>(0.15, 0.19)(rng);
    xpos = uniform_int_distribution<int>(0, dims - 1)(rng);
    ypos = uniform_int_distribution<int>(0, dims - 1)(rng);

  }

  void move(const vector<vector<double>> & landscape, vector<vector<int>> & presence);

  double food;
  double act;
  int xpos;
  int ypos;
};

void ind::move(const vector<vector<double>> & landscape, vector<vector<int>> & presence) {

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

bool check_IFD(const vector<ind> & pop, const vector < vector<double>> & landscape, const vector<vector<int>> & presence) {


  for (int p = 0; p < pop.size(); ++p) {
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

double count_IFD(const vector<ind> & pop, const vector < vector<double>> & landscape, const vector<vector<int>> & presence) {

  int count = pop.size();
  int p = 0;
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

void landscape_setup(vector<vector<double>> & landscape) {
  for (int i = 0; i < dims; ++i) {
    for (int j = 0; j < dims; ++j) {
      landscape[i][j] = uniform_real_distribution<double>(0.5, 1.0)(rng);
    }
  }
}

void reproduction(vector<ind> & pop) {

  vector<double> fitness;

  for (int i = 0; i < pop.size(); ++i) {
    fitness.push_back(max(pop[i].food - fcost * pop[i].act * run_time * num_scenes, 0.0));
  }

  rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
  rdist.mutate(fitness.cbegin(), fitness.cend());
  auto tmp_pop = pop;

  for (int i = 0; i < pop.size(); ++i) {
    const int ancestor = rdist(rng);
    tmp_pop[i] = pop[ancestor];
    tmp_pop[i].xpos = uniform_int_distribution<int>(0, dims - 1)(rng);
    tmp_pop[i].ypos = uniform_int_distribution<int>(0, dims - 1)(rng);
    tmp_pop[i].food = 0.0;

    if (bernoulli_distribution(mutation_rate)(rng)) {
      tmp_pop[i].act += normal_distribution<double>(0.0, mutation_shape)(rng);
      tmp_pop[i].act = max(tmp_pop[i].act, 0.0);
    }
  }

  //using std::swap;
  swap(pop, tmp_pop);
}


int main() {

  unsigned seed =
    static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
  std::clog << "random_seed : " << seed << '\n';
  rng.seed(seed);

  std::ofstream ofs1(ID_run + "activities.txt", std::ofstream::out);
  //std::ofstream ofs2("IDF2.txt", std::ofstream::out);
  //ofs2 << "G" << "\t" << "prop_ifd" << "\t" << "avg_ttifd" << "\t" << endl;

  std::ofstream ofs3(ID_run + "params.txt", std::ofstream::out);
  ofs3 << "dims" << "\t" << dims << "\n"
    << "pop_size" << "\t" << pop_size << "\n"
    << "Gmax" << "\t" << Gmax << "\n"
    << "run_time" << "\t" << run_time << "\n"
    << "num_scenes" << "\t" << num_scenes << "\n"
    << "mutation_rate" << "\t" << mutation_rate << "\n"
    << "mutation_shape" << "\t" << mutation_shape << "\n"
    << "fcost" << "\t" << fcost << "\n";

  ofs3.close();


  //landscape initialization
  vector<vector<double>> landscape(dims, vector<double>(dims, 1.0));
  landscape_setup(landscape);

  vector<ind> pop(pop_size);

  for (int g = 0; g < Gmax; ++g) {

    vector<double> activities;
    vector<vector<int>> presence(dims, vector<int>(dims, 0));
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

    for (int scenes = 0; scenes < num_scenes; ++scenes) {
      //cout << "scenes: " << scenes << endl;
      landscape_setup(landscape);

      double time = 0.0;
      int id;
      int eat_t = 0;
      bool IFD_reached = false;
      double time_to_IFD = 0.0;

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
        id = rdist(rng);
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

    if (g % 10 == 0) {
      ofs1 << g << "\t";
      for (int q = 0; q < pop.size(); q += pop.size() / 1000) {
        ofs1 << pop[q].act << "\t";
      }
      ofs1 << "\n";
      //ofs2 << g << "\t" << ifd_prop / num_scenes << "\t" << total_ttIFD / num_scenes << "\t\n";
    }

    reproduction(pop);
    cout << g << endl;
  }
  ofs1.close();
  //ofs2.close();
  cout << "End";

  return 0;
}