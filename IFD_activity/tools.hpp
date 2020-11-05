#pragma once

#include <fstream>
#include "parameters.hpp"
#include "agents.hpp"
#include "landscape.hpp"
#include <cassert>

// functions to print sim output

/// construct agent output filename
std::vector<std::string> identifyOutpath(const int pop_density,
    const double run_time,
    const int num_scenes, const double f_cost, const std::string rep_id,
    const std::string idRun) {
    
    // assumes the data folder exists
    std::string path = "data/";
    
    // output filename as milliseconds since epoch
    // write summary with filename to agent data
    // and parameter files
    // start with output id
    const std::string summary_out = path + "/lookup.csv";
    std::ofstream summary_ofs;

    // if not exists write col names
    std::ifstream f2(summary_out.c_str());
    if (!f2.good()) {
        summary_ofs.open(summary_out, std::ofstream::out);
        summary_ofs << "filename,pop_density,run_time,n_scenes,f_cost,rep_id\n";
        summary_ofs.close();
    }
    // append if not
    summary_ofs.open(summary_out, std::ofstream::out | std::ofstream::app);
    summary_ofs << idRun << ","
        << pop_density << ","
        << run_time << ","
        << num_scenes << ","
        << f_cost << ","
        << rep_id << "\n";
    summary_ofs.close();

    return std::vector<std::string> { path, idRun };
}

// tabulate acitivity in increments
std::vector<std::pair<double, int> > getActTable(std::vector<ind>& pop,
    const double valRound)
{
    // collect masses
    std::vector<double> popAct(pop.size());

    // round to the nearest multiple of massRound
    for (size_t p_i = 0; p_i < pop.size(); ++p_i)
    {
        popAct[p_i] = pop[p_i].act;
    }
    // round to the nearest multiple of massRound
    for (size_t p_i = 0; p_i < popAct.size(); ++p_i)
    {
        popAct[p_i] = round(popAct[p_i] / valRound) * valRound;
    }

    // make a copy
    std::vector<double> popActUnique = popAct;
    std::sort(popActUnique.begin(), popActUnique.end());
    popActUnique.erase(unique(popActUnique.begin(), popActUnique.end()),
        popActUnique.end());

    // count unique masses in range
    std::vector<std::pair<double, int> > massTable(popActUnique.size());

    for (size_t it = 0; it < popActUnique.size(); ++it)
    {
        massTable[it].first = popActUnique[it];
        const double thisVal = popActUnique[it];
        massTable[it].second = static_cast<int>(std::count_if(popAct.begin(), popAct.end(), [thisVal](double f) {
            return fabs(f - thisVal) < 0.000000001;
            }));
    }

    assert(massTable.size() > 0);
    return massTable;

}

// print summary activity
void printSummaryMass(std::vector<ind>& pop,
    const int gen,
    const double valRound,
    std::vector<std::string> outpath) {
    // ofstream
    std::ofstream rnormOfs;
    // std::cout << "data path = " << outpath[0] + outpath[1] << "\n";

    // check if okay
    std::ifstream f(outpath[0] + outpath[1] + "_summary.csv");
    // if (!f.good()) {
    //     std::cout << "data path " << outpath[0] + outpath[1] << " good to write\n";
    // }
    // write column names
    rnormOfs.open(outpath[0] + outpath[1] + "_summary.csv",
        std::ofstream::out | std::ofstream::app);

    if (gen == 0) {
        rnormOfs << "gen,actRound,count\n";
    }

    // get population mass summary stats
    std::vector<std::pair<double, int> > popSummaryAct = getActTable(pop, valRound);

    assert(popSummaryAct.size() > 0);

    // print mass counts
    for (size_t p_i = 0; p_i < popSummaryAct.size(); ++p_i)
    {
        rnormOfs << gen << ","
            << popSummaryAct[p_i].first << ","
            << popSummaryAct[p_i].second << "\n";
    }

    rnormOfs.close();
}
