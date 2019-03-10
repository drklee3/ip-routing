#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "costs.h"
#include "logger.h"
#include "machine.h"

/**
 * @brief Create a config object
 * 
 * @param name     Name of machine
 * @param ip       IP of machine
 * @param port     Port of machine
 * @return Config* 
 */
Config* create_config(Machine* machines, Machine* machine, CostTable* costs_tbl) {
    // malloc to allocate on heap
    Config* cfg = malloc(sizeof(Config));

    cfg->shutdown = malloc(sizeof(int));
    cfg->shutdown = 0; // set to nonzero to exit loops in other threads
    cfg->costs    = costs_tbl;

    // current machine
    cfg->machine = machine;

    cfg->machines = machines;

    return cfg;
}

/**
 * @brief Parses the machines file
 * 
 * @param fp        Pointer to machines file
 * @return Machine* Array of machines
 */
Machine* parse_machines(FILE* fp) {
    if (!fp) {
        log_error("Failed to read machines file");
        return 0;
    }

    Machine* machines = (Machine*) malloc(4 * sizeof(Machine));

    for (int i = 0; i < 4; ++i) {
        machines[i].id = i;
        fscanf(fp, "%s", machines[i].name);
        fscanf(fp, "%s", machines[i].ip);
        fscanf(fp, "%d", &machines[i].port);
    }

    return machines;
}

/**
 * @brief Prints all machine information
 * 
 * @param machines 
 */
void print_machines(Machine* machines) {
    for (int i = 0; i < 4; ++i) {
        printf("[%d] %s => %s:%d\n",
            machines[i].id,
            machines[i].name,
            machines[i].ip,
            machines[i].port);
    }
}

/**
 * @brief Determines if a node is a neighbor to the current machine
 * 
 * @param cfg      Configuration
 * @return size_t* array of neighbor ids
 */
size_t* get_neighbors(Config* cfg) {
    size_t* neighbors = (size_t*) malloc(4 * sizeof(size_t));
    size_t neighbors_i = 0;

    size_t** table = lock_table(cfg->costs);
    int curr_id = cfg->machine->id;

    for (int i = 0; i < 4; ++i) {
        // not neighbor
        if (table[i][curr_id] >= 100) {
            continue;
        }

        // append neighbor
        neighbors[neighbors_i] = i;
    }
    unlock_table(cfg->costs);

    return neighbors;
}

