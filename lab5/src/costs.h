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
    // current hop count for receive/send
    size_t           hop_count;
} CostTable;

size_t** parse_costs(FILE* fp);
CostTable* create_cost_table(FILE* fp, pthread_mutex_t* lock);
size_t** lock_table(CostTable* tbl);
void unlock_table(CostTable* tbl);
void print_costs(CostTable* tbl);
void update_costs(CostTable* tbl, int* msg);
int* get_least_costs(CostTable* tbl, int start);
void print_array(int* arr, int size);

#endif
