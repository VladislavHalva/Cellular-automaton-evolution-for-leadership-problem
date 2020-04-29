#include <iostream>
#include <ctime>
#include <cstdlib>
#include <tuple>

using namespace std;

class CA_leader_simulator{
    public:
        CA_leader_simulator(int cells_count, char* rules);
        tuple<int, int, int, int> run_simulation(int steps);
    private:
        int cells_count;
        char* rules;
        char* current_config;
        void apply_rule(char *next_config, int index);
        int count_ones(char *config);
        bool configs_identical(char *config1, char *config2);
};

//ctor
CA_leader_simulator::CA_leader_simulator(int cells_count, char* rules)
{
    this->cells_count = cells_count;
    this->rules = rules;
}

/**
 * The returned values represents <success, step, number of ones, constant_config>
 * Success and constant_config are treated ase boolean
 */
tuple<int, int, int, int> CA_leader_simulator::run_simulation(int steps){
    //random init state
    this->current_config = (char *)malloc(this->cells_count * sizeof(char));
    srand(time(NULL));
    for (int i = 0; i < this->cells_count; i++){
        this->current_config[i] = rand() % 2;
    }

    //transform cellular automaton in discrete steps according to rules
    for (int step = 0; step < steps; step++){
        char *next_config = (char *)malloc(this->cells_count * sizeof(char));

        //apply rules to all cells of automaton
        for (int i = 0; i < this->cells_count; i++){
            apply_rule(next_config, i);
        }

        // constant configuration -> states has not changed after rules application
        bool identical = this->configs_identical(this->current_config, next_config);
        if(identical){
            int ones = this->count_ones(next_config);
            if (ones == 1){
                //success -> leader found + constant configuration
                return make_tuple(1, step-1, ones, 1);
            }
            else{
                //fail -> leader not found, but constant configuration
                return make_tuple(0, step - 1, ones, 1);
            }
        }

        //overwrite state
        free(this->current_config);
        this->current_config = next_config;
    }

    //simulation finished without getting to constant configuration
    int ones = this->count_ones(this->current_config);
    free(this->current_config);
    return make_tuple(0, steps, ones, 0);
}

void CA_leader_simulator::apply_rule(char* next_config, int index){
    int cells = this->cells_count;

    //get the neighbourhood state as an integer in 0-127 (each cell represents one bit)
    int neigh_state = 0;
    if (index > 2) { neigh_state += this->current_config[index - 3] * 64; }
    if (index > 1) { neigh_state += this->current_config[index - 2] * 32; }
    if (index > 0) { neigh_state += this->current_config[index - 1] * 16; }
    neigh_state += this->current_config[index] * 8;
    if (index < cells-3) { neigh_state += this->current_config[index + 3]; }
    if (index < cells-2) { neigh_state += this->current_config[index + 2] * 2; }
    if (index < cells-1) { neigh_state += this->current_config[index + 1] * 4; }

    //use state of local neighbourhood as index of rule to be applied
    next_config[index] = this->rules[neigh_state];
}


/**
 * Counts ones in CA configuration
 */
int CA_leader_simulator::count_ones(char* config){
    int count = 0;
    for (int i = 0; i < this->cells_count; i++)
    {
        if(config[i] == 1) {count++;}
    }
    return count;
}

/**
 * Determines if two given cofigurations are identical
 */
bool CA_leader_simulator::configs_identical(char* config1, char* config2){
    for (int i = 0; i < this->cells_count; i++){
        if(config1[i] != config2[i]) { return false;}
    }
    return true;
}