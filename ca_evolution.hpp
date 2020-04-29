#include "ca_simulator.hpp"
#include <cstring>
#include <queue>
#include <cmath>
#include "additional.hpp"

class CA_leader_evolution{
    public:
        CA_leader_evolution(int cells, int generations, int population_size, double mutationP, int lambda, int configs_for_fitness);
        int run();
        int* calculate_fitness(char **population);
        int* get_elite_individuals_indexes(int* population_fitness);

    private:
        int cells;
        int steps;
        int population_size;
        double mutationP;
        int lambda;
        int rules_count;
        int generations;
        int configs_for_fitness;
        int elite_size;
        char *single_point_cross(char *rule1, char *rule2);
        char *two_point_cross(char *rule1, char *rule2);
        char *uniform_cross(char *rule1, char *rule2);
        void mutate(char *rules);
};

//ctor
CA_leader_evolution::CA_leader_evolution(int cells, int generations, int population_size, double mutationP, int lambda, int configs_for_fitness)
{
    this->cells = cells;
    this->population_size = population_size;
    this->mutationP = mutationP;
    this->lambda = lambda;
    this->generations = generations;
    this->configs_for_fitness = configs_for_fitness;

    this->steps = cells * 2;
    this->rules_count = 128;
    this->elite_size = round(population_size / 5); // 20%
}

int CA_leader_evolution::run(){

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
    int *parents_fitness = this->calculate_fitness(population);

    for (int gen = 0; gen < this->generations; gen++){
        //create mew population
        char **new_population = (char **)malloc(sizeof(char *) * this->population_size);

        //find elite (few percent of best individuals)
        int *elite_indices = this->get_elite_individuals_indexes(parents_fitness);

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
        for (int i = this->elite_size; i < this->population_size; i++){
            //choose two random members of elite
            srand(time(NULL));
            int elite1 = rand() % this->elite_size;
            int elite2 = rand() % this->elite_size;

            //crossover
            new_population[i] = this->single_point_cross(new_population[elite1], new_population[elite2]);
        }

        //mutation
        for(int i = 0; i < this->population_size; i++){
            this->mutate(new_population[i]);
        }

        population = new_population;
        free(parents_fitness);
        parents_fitness = this->calculate_fitness(population);


        cout << "Generation: " << gen << ", max fitness: " << maxInArray(parents_fitness, this->population_size) << "\n";
    }

    //find best solution
    int *elite = this->get_elite_individuals_indexes(parents_fitness);
    char *best = population[elite[0]];
    for (int i = 0; i < this->rules_count; i++){
        printf("%d ", best[i]);
    }
    printf("\n");

    free(parents_fitness);
    for (int i = 0; i < this->population_size; i++)
    {
        free(population[i]);
    }
    free(population);
    return 1;
}

int* CA_leader_evolution::get_elite_individuals_indexes(int *population_fitness){
    priority_queue<pair<int, int>> q;
    for(int i = 0; i < this->population_size; i++){
        q.push(pair<int, int>(population_fitness[i], i));
    }
    int *elite_indices = (int *)malloc(sizeof(int) * this->elite_size);
    for (int i = 0; i < this->elite_size; i++){
        elite_indices[i] = q.top().second;
    }
    return elite_indices;
}

int* CA_leader_evolution::calculate_fitness(char **population){
    int *population_fitness = (int *)malloc(sizeof(int) * this->population_size);

    //for all rules sets in population calc fitness
    for (int individial = 0; individial < this->population_size; individial++){
        //init simulator
        double fitness = 0;
        auto sim = CA_leader_simulator(this->cells, population[individial]);

        //run defined number of simulations
        for (int simId = 0; simId < this->configs_for_fitness; simId++){
            int success, step, ones, constant;
            tie(success, step, ones, constant) = sim.run_simulation(this->steps);

            fitness += (this->cells - ones); //TODO edit fitness
        }

        fitness /= this->configs_for_fitness;
        population_fitness[individial] = fitness;
    }

    return population_fitness;
}

char* CA_leader_evolution::single_point_cross(char* rule1, char* rule2){
    srand(time(NULL));
    int crossPoint = rand() % this->rules_count;

    char *res = (char *)malloc(sizeof(char) * this->rules_count);
    memcpy(res, rule1, crossPoint);
    memcpy(res + crossPoint, rule2, this->rules_count - crossPoint);
    return res;
}

char *CA_leader_evolution::two_point_cross(char *rule1, char *rule2){
    srand(time(NULL));
    int cp1 = rand() % this->rules_count;
    int cp2 = rand() % this->rules_count;
    if (cp2 < cp1) { swap(&cp1, &cp2); }

    char *res = (char *)malloc(sizeof(char) * this->rules_count);
    memcpy(res, rule1, cp1);
    memcpy(res + cp1, rule2 + cp1, cp2 - cp1);
    memcpy(res + cp1 + cp2, rule1 + cp1 + cp2, this->rules_count - cp2);
    return res;
}

char *CA_leader_evolution::uniform_cross(char *rule1, char *rule2){
    srand(time(NULL));
    char *res = (char *)malloc(sizeof(char) * this->rules_count);

    for (int i = 0; i < this->rules_count; i++){
        int origin = rand() % 2;
        if(origin == 0){
            res[i] = rule1[i];
        }
        else{
            res[i] = rule2[i];
        }
    }
    return res;
}

void CA_leader_evolution::mutate(char* rules){
    srand(time(NULL));
    for (int i = 0; i < this->rules_count; i++){
        double r = ((double)rand() / (RAND_MAX));
        if(r > this->mutationP){
            rules[i] = 1 - rules[i];
        }
    }
}