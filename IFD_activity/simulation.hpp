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


template <typename T>
class Grid {
public:
  explicit Grid(int dim) : dim_(dim), buf_(size_t(dim)* dim) {}
  Grid(int dim, const T& val) : dim_(dim), buf_(size_t(dim)* dim, val) {}

  // access to underlying linear buffer
  const auto& buf() const noexcept { return buf_; }
  auto& buf() noexcept { return buf_; }

  // 2D stuff
  int dim() const noexcept { return dim_; }
  auto& operator()(int x, int y) { return buf_[size_t(x) * dim_ + y]; }
  const auto& operator()(int x, int y) const { return buf_[size_t(x) * dim_ + y]; }

  // conversion linear <-> 2D
  size_t linear_idx(int x, int y) const noexcept { return size_t(x) * dim_ + y; }
  std::pair<int, int> coor(size_t idx) const noexcept {
    return { static_cast<int>(idx) % dim_, static_cast<int>(idx) / dim_ };
  }

private:
  int dim_;
  std::vector<T> buf_;   // linear buffer
};



using landscape_t = Grid<cell>;
using presence_t = Grid<double>;


class iota_sampler_t {
public:
  explicit iota_sampler_t(size_t i) : iotas_(i) {
    std::iota(iotas_.begin(), iotas_.end(), 0);
  }

  template <typename Reng>
  const std::vector<int>& operator()(Reng& reng) {
    std::shuffle(iotas_.begin(), iotas_.end(), reng);
    return iotas_;
  }

private:
  std::vector<int> iotas_;
};


struct ind {

  ind() {}
  ind(int x, int y, double a, double c, double b) : xpos(x), ypos(y), act(a), comp(c), bold(b) {}

  void die(double& presence);
  void mutate(bernoulli_distribution& mutate, normal_distribution<double>& mshape);

  void move(const landscape_t& landscape, presence_t& presence, Param param_, const std::vector<int>& iota);

  void springoff(const ind& parent);
  double updateintake(const landscape_t& landscape, presence_t& presence);


  bool dead = false;
  double food = 0.0;
  double act;
  double comp;
  double bold;
  int xpos;
  int ypos;
};


double ind::updateintake(const landscape_t& landscape, presence_t& presence) {
  return landscape(xpos, ypos).resource * comp / presence(xpos, ypos) /*+ bold * landscape[xpos][ypos].risk*/;

}

void ind::move(const landscape_t& landscape, presence_t& presence, Param param_, const std::vector<int>& iota) {

  if (!dead) {
    double present_intake = landscape(xpos, ypos).resource * comp / (presence(xpos, ypos)) /*+ bold * landscape[xpos][ypos].risk*/;
    double potential_intake;
    int former_xpos = xpos;
    int former_ypos = ypos;

    //std::uniform_real_distribution<double>error (0.0, 1.0);
    size_t newidx = std::uniform_int_distribution<size_t>(0, landscape.buf().size() - 1)(rnd::reng);
    //for (int i = 0; i < landscape.size(); ++i) {
    //  for (int j = 0; j < landscape[i].size(); ++j) {
        potential_intake = landscape.buf()[newidx].resource * comp / (presence.buf()[newidx] + comp);
        if (present_intake < potential_intake) {
          present_intake = potential_intake;
          const auto newpos = landscape.coor(newidx);
          xpos = newpos.first;
          ypos = newpos.second;
        }
    //  }
    //}


        auto bestidx = landscape.linear_idx(xpos, ypos);
        for (int i = 0; i < param_.nrexplore; ++i) {
          const auto idx = iota[i];
          potential_intake = landscape.buf()[idx].resource * comp / (presence.buf()[idx] + comp);
          if (present_intake < potential_intake) {
            present_intake = potential_intake;
            bestidx = idx;
          }

        }
    presence.buf()[bestidx] += comp;
    presence(former_xpos, former_ypos) -= comp;
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


bool check_IFD(const vector<ind>& pop, const landscape_t& landscape, const presence_t& presence) {


  for (const auto& ind : pop) {
    const auto present_intake = landscape(ind.xpos, ind.ypos).resource * ind.comp / presence(ind.xpos, ind.ypos);
    for (int i = 0; i < landscape.buf().size(); ++i) {
      if (present_intake < landscape.buf()[i].resource * ind.comp / (presence.buf()[i] + ind.comp)) {
        return false;
      }
    }
  }
  return true;
}

double count_IFD(const vector<ind>& pop, const landscape_t& landscape, const presence_t& presence) {

  size_t count = pop.size();
  for (const auto& i : pop) {
    const auto present_intake = landscape(i.xpos, i.ypos).resource * i.comp / presence(i.xpos, i.ypos);
    for (auto c = 0; c < landscape.buf().size(); ++c) {
      if (present_intake < landscape.buf()[c].resource * i.comp / (presence.buf()[c] + i.comp)) {
        --count;
        break;
      }
    }
  }
  return static_cast<double>(count) / pop.size();
}

double intake_variance(const vector<ind>& pop, const landscape_t& landscape, const presence_t& presence) {
  auto lambda = [&](const ind& i) {
    return landscape(i.xpos, i.ypos).resource * i.comp / presence(i.xpos, i.ypos);
  };
  const double sum = std::accumulate(pop.cbegin(), pop.cend(), 0.0, [&](double s, const ind& i) {
    return s + lambda(i);
  });
  const auto mean = sum / pop.size();
  const double accum = std::accumulate(pop.cbegin(), pop.cend(), 0.0, [&](double s, const ind& i) {
    const auto val = lambda(i);
    return s + (val - mean) * (val - mean);
  });
  return sqrt(accum / pop.size());
}

void landscape_setup(landscape_t& landscape, Param param_) {
  for (cell& c : landscape.buf()) {
      c = cell(uniform_real_distribution<double>(param_.resource_min, param_.resource_max)(rnd::reng),
        /*exponential_distribution<double>(param_.riskspread)(rnd::reng)*/0.0);
  }
}

void reproduction(vector<ind>& pop, vector<ind>& tmp_pop, const Param& param_) {

  //vector<double> fitness;

  //for (int i = 0; i < pop.size(); ++i) {
  //  fitness.push_back(max(0.0, pop[i].food - param_.cost * pop[i].act * param_.t_scenes - param_.cost_comp * pop[i].comp * param_.t_scenes));
  //}

  //rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
  //rdist.mutate(fitness.cbegin(), fitness.cend());

  rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
  rdist.mutate_transform(pop.cbegin(), pop.cend(), [&](const ind& i) {
    return max(0.0, i.food - param_.cost * i.act * param_.t_scenes - param_.cost_comp * i.comp * param_.t_scenes);
  });

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
  pop.swap(tmp_pop);
}


void simulation(const Param& param_) {

  if (param_.seed != 0)
    rnd::reng.seed(param_.seed);



  std::ofstream ofs1(param_.outdir + "activities.txt", std::ofstream::out);
  std::ofstream ofs2(param_.outdir + "ecology.txt", std::ofstream::out);
  std::ofstream ofs3(param_.outdir + "comp.txt", std::ofstream::out);
  std::ofstream ofs4(param_.outdir + "bold.txt", std::ofstream::out);
  //std::ofstream ofs5(param_.outdir + "landscape.txt", std::ofstream::out);
  std::ofstream ofs5(param_.outdir + "_landscape.txt", std::ofstream::out);

  ofs5 << "gen\tscene\ttime\tcomp\tact\txpos\typos\tfood\tintake\n";


  ofs2 << "G" << "\t" << "ifd_prop" << "\t" << "time_to_IFD" << "\t" << "sd_intake" << "\t\n";


  std::ofstream os(param_.outdir + "params.txt", std::ofstream::out);
  stream_parameter(os, param_, "  ", ",\n", "c(", ")");
  os.close();

  //landscape initialization
  landscape_t landscape(param_.dims);



  vector<ind> pop;
  vector<ind> tmp_pop(param_.pop_size);
  auto pdist = std::uniform_int_distribution<int>(0, param_.dims - 1);
  for (int i = 0; i < param_.pop_size; ++i) {
    pop.emplace_back(pdist(rnd::reng), pdist(rnd::reng), 1, 1.7, 0.0);
  }
  auto iota_sampler = iota_sampler_t(landscape.buf().size());
  rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni> rdist;
  vector<double> activities;

  for (int g = 0; g < param_.G; ++g) {
    activities.clear();
    for (int i = 0; i < pop.size(); ++i) {
      activities.push_back(pop[i].act);
      //presence[pop[i].xpos][pop[i].ypos] += pop[i].comp;
    }
    activities.push_back(param_.changerate); 
    rdist.mutate(activities.cbegin(), activities.cend());
    double total_act = std::accumulate(activities.begin(), activities.end(), 0.0);
    exponential_distribution<double> event_dist(total_act);

    double ifd_prop = 0.0;
    double total_ttIFD = 0.0;
    double total_sdintake = 0.0;

    presence_t presence(param_.dims, 0.0);
    for (int i = 0; i < pop.size(); ++i) {
      presence(pop[i].xpos, pop[i].ypos) = pop[i].comp;
    }
    landscape_setup(landscape, param_);

    double time = 0.0;
    int id;
    int eat_t = 0;
    double it_t = 0.0;
    double increment = 0.1;
    bool IFD_reached = false;
    double time_to_IFD = 0.0;
    int count = 0;

    for (; time < param_.t_scenes; ) {

      time += event_dist(rnd::reng);

      while (time > eat_t) { // alternative: individuals eat continuously. Maybe let's not
        for (int p = 0; p < pop.size(); ++p) {
          if (!pop[p].dead) {
            pop[p].food += landscape(pop[p].xpos, pop[p].ypos).resource * pop[p].comp / presence(pop[p].xpos, pop[p].ypos);
            //if(bernoulli_distribution(landscape[pop[p].xpos][pop[p].ypos].risk)(rnd::reng)) 
            //  pop[p].die(presence[pop[p].xpos][pop[p].ypos]);
          }
        }



        ++eat_t;


        if (g == param_.G - 1) {


          for (int i = 0; i < pop.size(); ++i) {

            ofs5 << g << "\t" << "\t" << eat_t << "\t" << pop[i].comp << "\t" << pop[i].act << "\t" << pop[i].xpos << "\t"
              << pop[i].ypos << "\t" << pop[i].food << "\t" << pop[i].updateintake(landscape, presence) << "\n";

          }
        }
      }

      id = rdist(rnd::reng);
      if (id == pop.size())
        ++count;
      
      else if (!IFD_reached) {
        //int xpos = pdist(rnd::reng);
        //int ypos = pdist(rnd::reng);
        pop[id].move(landscape, presence, param_, iota_sampler(rnd::reng));
        
        if (time > it_t) {
          IFD_reached = check_IFD(pop, landscape, presence);
          time_to_IFD = time;
          it_t = floor(time / increment) * increment + increment;
        }
      }
      if (count == param_.alpha)
      {
        count = 0;
        int nrcells = static_cast<int>(round(param_.dims * param_.dims * param_.changeprop));

        auto& iota = iota_sampler(rnd::reng);
        for (int i = 0; i < nrcells; ++i) {
          landscape.buf()[iota[i]] = cell(uniform_real_distribution<double>(param_.resource_min, param_.resource_max)(rnd::reng), 0.0);
        }
      }



    }
    //prop idf fulfilled
    ifd_prop += count_IFD(pop, landscape, presence);
    total_ttIFD += time_to_IFD;
    total_sdintake += intake_variance(pop, landscape, presence);  //Correct intake for competitiveness?

    if (g % 1 == 0) {
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
      ofs2 << g << "\t" << ifd_prop << "\t" << total_ttIFD << "\t" << total_sdintake  << "\t\n";
    }

    reproduction(pop, tmp_pop, param_);
    cout << g << endl;
  }
  ofs1.close();
  ofs2.close();
  cout << "End";



}
