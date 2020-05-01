#include "ca_simulator.hpp"
#include <cstring>
#include <queue>
#include <cmath>
#include "additional.hpp"

class CA_leader_evolution{
    public:
        CA_leader_evolution(int cells, int generations, int population_size, double mutationP, int configs_for_fitness);
        tuple<int,float,char*> run();
        float* calculate_fitness(char **population);
        int* get_elite_individuals_indices(float* population_fitness);

    private:
        int cells;
        int steps;
        int population_size;
        double mutationP;
        int rules_count;
        int generations;
        int configs_for_fitness;
        int elite_size;
        tuple<char*, char*> single_point_cross(char *rule1, char *rule2);
        tuple<char*, char*> two_point_cross(char *rule1, char *rule2);
        tuple<char*, char*> uniform_cross(char *rule1, char *rule2);
        void mutate(char *rules);
        void mutateTwoRandomRules(char *rules);
};

//ctor
CA_leader_evolution::CA_leader_evolution(int cells, int generations, int population_size, double mutationP, int configs_for_fitness)
{
    this->cells = cells;
    this->population_size = population_size;
    this->mutationP = mutationP;
    this->generations = generations;
    this->configs_for_fitness = configs_for_fitness;

    this->steps = cells * 2;
    this->rules_count = 128;
    this->elite_size = round(population_size / 5); // 20%
}

tuple<int,float,char*> CA_leader_evolution::run(){

    char *best_rules = (char *)malloc(sizeof(char) * this->rules_count);
    float best_fitness = 0;
    int best_generation = 0;

    //creation of random initial population_size of rules
    srand(time(NULL));
    char **population = (char **)malloc(sizeof(char *) * this->population_size);
    for(int i = 0; i < this->population_size; i++){
        population[i] = (char*) malloc(sizeof(char) * this->rules_count);
        for (int j = 0; j < this->rules_count; j++){
            population[i][j] = rand() % 2;
        }
    }

    //calculate fitness for population
    float *fitness = this->calculate_fitness(population);

    for (int gen = 0; gen < this->generations; gen++){
        //create mew population
        char **new_population = (char **)malloc(sizeof(char *) * this->population_size);

        //find elite (few percent of best individuals)
        int *elite_indices = this->get_elite_individuals_indices(fitness);

        //store best individual
        if(fitness[elite_indices[0]] > best_fitness){
            best_fitness = fitness[elite_indices[0]];
            best_generation = gen;
            memcpy(best_rules, population[elite_indices[0]], this->rules_count);

            if(best_fitness > 0.99){
                free(elite_indices);
                free(new_population);
                break;
            }
        }

        //copy elite to next generation
        for (int i = 0; i < this->elite_size; i++){
            new_population[i] = (char *)malloc(sizeof(char) * this->rules_count);
            memcpy(new_population[i], population[elite_indices[i]], this->rules_count);
        }

        //free old population
        for (int i = 0; i < this->population_size; i++) { free(population[i]); }
        free(population);
        free(elite_indices);

        //create the rest of the new population using crossover with randomly chosen elite members
        for (int i = this->elite_size; i < this->population_size; i+=2){
            //choose two random members of elite
            srand(time(NULL));
            int elite1 = rand() % this->elite_size;
            int elite2 = rand() % this->elite_size;

            //crossover
            char* child1;
            char *child2;

            tie(child1, child2) = this->two_point_cross(new_population[elite1], new_population[elite2]);
            if(i < this->population_size-1){
                //at leats two free spots in population
                new_population[i] = child1;
                new_population[i + 1] = child2;
            }
            else{
                //only one free spot left 
                new_population[i] = child1;
            }
        }

        //mutation
        for(int i = 0; i < this->population_size; i++){
            // this->mutate(new_population[i]);
            this->mutateTwoRandomRules(new_population[i]);
        }

        population = new_population;
        free(fitness);
        fitness = this->calculate_fitness(population);

        fprintf(stderr, "generation: %d, max fitness: %f\n", gen, maxInArray(fitness, this->population_size));
    }

    free(fitness);
    for (int i = 0; i < this->population_size; i++)
    {
        free(population[i]);
    }
    free(population);
    return make_tuple(best_generation, best_fitness, best_rules);
}

int* CA_leader_evolution::get_elite_individuals_indices(float *population_fitness){
    priority_queue<pair<float, int>> q;
    for(int i = 0; i < this->population_size; i++){
        q.push(pair<float, int>(population_fitness[i], i));
    }
    int *elite_indices = (int *)malloc(sizeof(int) * this->elite_size);
    for (int i = 0; i < this->elite_size; i++){
        elite_indices[i] = q.top().second;
    }
    return elite_indices;
}

float* CA_leader_evolution::calculate_fitness(char **population){
    float *population_fitness = (float *)malloc(sizeof(float) * this->population_size);

    //for all rules sets in population calc fitness
    for (int individial = 0; individial < this->population_size; individial++){
        //init simulator
        float fitness = 0.0;
        auto sim = CA_leader_simulator(this->cells, population[individial]);

        //run defined number of simulations
        for (int simId = 0; simId < this->configs_for_fitness; simId++){
            int success, step, ones, constant;
            tie(success, step, ones, constant) = sim.run_simulation(this->steps);

            // if (success == 1){
            //     //constant configuration with exactly one 1
            //     fitness += this->cells * 2;
            // }
            // else{
            //     //else fittness = cells - abs(1-ones) -> max for one 1
            //     fitness += this->cells - abs(1 - ones);
            // }         
            if (success == 1){
                fitness += 1.0;
            }
        }

        //fitness is average fitness per all runs with tested set of rules
        fitness /= this->configs_for_fitness;
        population_fitness[individial] = fitness;
    }

    return population_fitness;
}

tuple<char*,char*> CA_leader_evolution::single_point_cross(char *rule1, char *rule2)
{
    srand(time(NULL));
    int crossPoint = rand() % this->rules_count;

    char *res1 = (char *)malloc(sizeof(char) * this->rules_count);
    char *res2 = (char *)malloc(sizeof(char) * this->rules_count);

    memcpy(res1, rule1, crossPoint);
    memcpy(res1 + crossPoint, rule2, this->rules_count - crossPoint);

    memcpy(res2, rule2, crossPoint);
    memcpy(res2 + crossPoint, rule1, this->rules_count - crossPoint);

    return make_tuple(res1, res2);
}

tuple<char*,char*> CA_leader_evolution::two_point_cross(char *rule1, char *rule2)
{
    srand(time(NULL));
    int cp1 = rand() % this->rules_count;
    int cp2 = rand() % this->rules_count;
    if (cp2 < cp1) { swap(&cp1, &cp2); }

    char *res1 = (char *)malloc(sizeof(char) * this->rules_count);
    char *res2 = (char *)malloc(sizeof(char) * this->rules_count);

    memcpy(res1, rule1, cp1);
    memcpy(res1 + cp1, rule2 + cp1, cp2 - cp1);
    memcpy(res1 + cp2, rule1 + cp2, this->rules_count - cp2);

    memcpy(res2, rule2, cp1);
    memcpy(res2 + cp1, rule1 + cp1, cp2 - cp1);
    memcpy(res2 + cp2, rule2 + cp2, this->rules_count - cp2);

    return make_tuple(res1, res2);
}

tuple<char*,char*> CA_leader_evolution::uniform_cross(char *rule1, char *rule2)
{
    srand(time(NULL));
    char *res1 = (char *)malloc(sizeof(char) * this->rules_count);
    char *res2 = (char *)malloc(sizeof(char) * this->rules_count);

    for (int i = 0; i < this->rules_count; i++){
        int origin = rand() % 2;
        if(origin == 0){
            res1[i] = rule1[i];
            res2[i] = rule2[i];
        }
        else{
            res1[i] = rule2[i];
            res2[i] = rule1[i];
        }
    }
    return make_tuple(res1,res2);
}

void CA_leader_evolution::mutate(char* rules){
    srand(time(NULL));
    for (int i = 0; i < this->rules_count; i++){
        double r = ((double)rand() / (RAND_MAX));
        if(r < this->mutationP){
            rules[i] = 1 - rules[i];
        }
    }
}

void CA_leader_evolution::mutateTwoRandomRules(char *rules){
    srand(time(NULL));
    int firstRuleId = rand() % this->rules_count;
    int secondRuleId = rand() % this->rules_count;

    //toggle two randomly chosen rules results
    rules[firstRuleId] = 1 - rules[firstRuleId];
    rules[secondRuleId] = 1 - rules[secondRuleId];
}