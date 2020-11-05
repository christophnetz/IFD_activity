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
    
    std::vector<std::string> cliArgs(argv, argv + argc);

    doSimulation(cliArgs);
    
    return 0;
}