#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "costs.h"
#include "logger.h"
#include "machine.h"
#include "receiver.h"
#include "sender.h"
#include "updater.h"

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s <machine id> <num machines> <costs file> <machines file>\n", argv[0]);
        return 1;
    }

    // parse machines file
    log_debug("fetching machines");
    FILE* machines_fp = fopen(argv[4], "r");
    Machine* machines = parse_machines(machines_fp);

    // check if machine parsing errored
    if  (!machines) {
        return 1;
    }

    print_machines(machines);

    // get current machine
    char* endptr; // used to check if strtol failed
    int machine_id = strtol(argv[1], &endptr, 10);
    if (machine_id > 3 || endptr <= argv[1]) {
        log_error("Invalid machine ID, must be between 0 and 3 inclusive");
        return 1;
    }

    Machine* curr_machine = &machines[machine_id];
    log_debug("current machine: %d => %s [%s:%d]",
        curr_machine->id, curr_machine->name,
        curr_machine->ip, curr_machine->port);

    // create neighbor cost table
    log_debug("creating neighbor cost table");
    FILE* costs_fp = fopen(argv[3], "r");
    pthread_mutex_t table_lock;
    CostTable* cost_table = create_cost_table(costs_fp, &table_lock);

    // check if cost table creation errored
    if (!cost_table) {
        return 1;
    }

    print_costs(cost_table);

    // create config
    log_debug("creating config");    
    Config* cfg = create_config(machines, curr_machine, cost_table);

    // reference to thread
    pthread_t thread_receiver;
    // create receiver thread #1
    log_debug("creating receiver thread");
    if (pthread_create(&thread_receiver, NULL, run_receiver, cfg)) {
        log_error("Error creating thread");
        return 1;
    }

    // create updater thread #3
    pthread_t thread_updater;
    log_debug("creating updater thread");
    if (pthread_create(&thread_updater, NULL, run_updater, cfg)) {
        log_error("Error creating thread");
        return 1;
    }

    read_changes(cfg);
    
    // join other threads
    if (pthread_join(thread_receiver, NULL)) {
        log_error("Failed to join receiver thread");
        return 1;
    }

    if (pthread_join(thread_updater, NULL)) {
        log_error("Failed to join updater thread");
        return 1;
    }

    return 0;
}
