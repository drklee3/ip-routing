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

typedef struct {
    size_t table[4][4];
    size_t hop_count;
} CostTableCopy;

size_t** parse_costs(FILE* fp);
CostTable* create_cost_table(FILE* fp, pthread_mutex_t* lock);
size_t** lock_table(CostTable* tbl);
void unlock_table(CostTable* tbl);
void print_costs(CostTable* tbl);
void update_costs(CostTable* tbl, CostTable* msg);
int* get_least_costs(CostTable* tbl, int start);
void print_array(int* arr, int size);

CostTable* cost_table_from_copy(CostTableCopy* cpy);
CostTableCopy* cost_table_to_copy(CostTable* tbl);

#endif
