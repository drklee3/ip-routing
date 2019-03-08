#ifndef MACHINE_H
#define MACHINE_H

#include <pthread.h>
#include "costs.h"

/**
 * @brief Machine configuration
 *  
 */
typedef struct {
    // index of machine in machines array
    int  id;
    char name[50];
    char ip[50];
    int  port;
} Machine;

/**
 * @brief Configuration options
 * 
 * Stores machine info as well as configuration settings for other parts.
 * This also contains shared state variables passed between threads.
 * 
 */
typedef struct {
    // this specific machine
    Machine*   machine;

    // all machines
    Machine*   machines;

    // shutdown status
    int*       shutdown;

    // cost table
    CostTable* costs;
} Config;

Config* create_config(Machine* machines, Machine* machine, CostTable* costs_tbl);
Machine* parse_machines(FILE* fp);
void print_machines(Machine* machines);

#endif
