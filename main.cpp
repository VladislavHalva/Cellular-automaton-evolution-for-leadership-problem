#include <iostream>
#include "ca_evolution.hpp"

using namespace std;

#define PRINT_ONE_VERIFICATION_RUN false

float verify_rules(char* rules){
    int verification_cells = 599;
    int steps = verification_cells * 2;
    int verification_configs_for_fitness = 10000;

    auto simulator = CA_leader_simulator(verification_cells, rules);

    float success_rate = 0.0;
    for (int i = 0; i < verification_configs_for_fitness; i++)
    {
        int success, step, ones, constant;
        tie(success,step,ones,constant) = simulator.run_simulation(steps);
        if (success == 1){
            success_rate += 1.0;
        }
    }
    success_rate /= verification_configs_for_fitness;
    return success_rate;
}

int main(int argc, char** argv){

    int cells = 149;
    int generations = 100;
    int population_size = 100;
    double mutationP = 0.05;
    int configs_for_fitness = 100;
    int rules_count = 128;

    //run evolution
    auto evolution = CA_leader_evolution(
        cells,
        generations,
        population_size,
        mutationP,
        configs_for_fitness);

        int best_generation;
        float best_fitness;
        char *best_rules;
        tie(best_generation, best_fitness, best_rules) = evolution.run();

    fprintf(stderr, "Evolution best fitness: %f, generation: %d\n", best_fitness, best_generation);

    //run evaluation 
    float v_succ_rate;
    v_succ_rate = verify_rules(best_rules);
    fprintf(stderr, "Verification success rate: %f", v_succ_rate);

    //show best solution
    fprintf(stderr, "Best rules: \n");
    for (int j = 0; j < rules_count; j++)
    {   
        if(j < rules_count-1){
            fprintf(stderr, "%u ", best_rules[j]);
        }
        else{
            fprintf(stderr, "%u", best_rules[j]);
        }

    }
    fprintf(stderr, "\n");

    if(PRINT_ONE_VERIFICATION_RUN){
        int ver_cells = 599;
        int ver_steps = ver_cells * 2;
        auto sim = CA_leader_simulator(ver_cells, best_rules);
        char** configs = sim.run_simulation_ret_transitions(ver_steps);

        for (int config = 0; config < ver_steps; config++)
        {
            for (int cell = 0; cell < ver_cells; cell++){
                if(cell < ver_cells-1){
                    printf("%u ", configs[config][cell]);
                }
                else{
                    printf("%u", configs[config][cell]);
                }
            }
            printf("\n");
        }
    }
    else{
        printf("%d, %f, %f\n", best_generation, best_fitness, v_succ_rate);
    }

    free(best_rules);
    return 0;
}