#ifndef COSTS_H
#define COSTS_H

#include <pthread.h>
#include <stdio.h>

/**
 * @brief Cost Table
 * 
 * Contains both the table itself and the mutex that protects the shared table
 * 
 */
typedef struct {
    pthread_mutex_t* lock;
    // inner actual cost table
    size_t**         table;
} CostTable;

size_t** parse_costs(FILE* fp);
CostTable* create_cost_table(FILE* fp, pthread_mutex_t* lock);
void print_costs(CostTable* tbl);
void update_costs(CostTable* tbl, int* msg);
int* get_least_costs(CostTable* tbl, int start);
void print_array(int* arr, int size);

#endif
