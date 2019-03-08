#include <stdio.h>
#include <unistd.h>
#include "costs.h"
#include "logger.h"
#include "machine.h"
#include "util.h"

/**
 * @brief Updater loop (Thread #3)
 * 
 * Repeats forever after random delays, runs the algorithm to update the least
 * costs and displays the least costs at the time.
 * 
 * @param _cfg   Configuration
 * @return void* 
 */
void* run_updater(void* _cfg) {
    Config cfg = *(Config*) _cfg;

    while (!cfg.shutdown) {
        // sleep random secs 10-20
        size_t sec = rand_range(10, 20);
        log_info("Sleeping for %d seconds", sec);
        sleep(sec);

        log_debug("current cost table:");
        print_costs(cfg.costs);
        int* costs = get_least_costs(cfg.costs, cfg.machine->id);
        printf("Least cost array: ");
        print_array(costs, 4);
        // costs array will be redeclared/reallocated within get_least_costs
        // so we want to free it to prevent memory leaks
        free(costs);
    }

    return 0;
}