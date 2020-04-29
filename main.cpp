#include <iostream>
#include "ca_evolution.hpp"

using namespace std;

int main(int argc, char** argv){

    int cells = 149;
    int generations = 200;
    int population_size = 20;
    double mutationP = 0.1;
    int lambda = 5;
    int configs_for_fitness = 100;
    

    auto evolution = CA_leader_evolution(
        cells,
        generations,
        population_size,
        mutationP,
        lambda,
        configs_for_fitness);

    evolution.run();

    return 0;
}