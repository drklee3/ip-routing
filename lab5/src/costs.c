#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "costs.h"
#include "logger.h"

/**
 * @brief Creates a cost table object
 * 
 * @param fp          Pointer to cost file
 * @param lock        Mutex for cost table
 * @return CostTable* 
 */
CostTable* create_cost_table(FILE* fp, pthread_mutex_t* lock) {
    if (!fp) {
        log_error("Failed to read costs file");
        return 0;
    }

    if (pthread_mutex_init(lock, NULL) != 0) { 
        log_error("Failed to initialize mutex"); 
        return 0;
    }

    size_t** table = parse_costs(fp);

    // create table struct pointer
    CostTable* cost_table = malloc(sizeof(CostTable));

    cost_table->lock  = lock;
    cost_table->table = table;

    return cost_table;
}

/**
 * @brief Parses the cost file
 * 
 * @param fp        File pointer to cost file
 * @return size_t** 2d array / matrix of costs
 */
size_t** parse_costs(FILE* fp) {
    // use malloc to store the matrix on the heap instead of
    // stack as to allow for use after exiting function scope
    size_t** costs_table = (size_t**) malloc(4 * sizeof(size_t*));

    // allocate inner arrays
    for (int i = 0; i < 4; ++i) {
        costs_table[i] = (size_t*) malloc(4 * sizeof(size_t));
    }

    // read file
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            fscanf(fp, "%zd", &costs_table[i][j]);
        }
    }

    fclose(fp);

    return costs_table;
}

/**
 * @brief Locks the table and returns inner cost table
 * 
 * This is used to avoid using the inner CostTable->table data structure
 * directly, and only using the returned table pointer from this function which
 * reduces the risk of accidentally using the inner table datastructure without
 * locking it first.  Only need to remember to call unlock_table(tbl) when
 * finished with the lock.
 * 
 * @param tbl       CostTable
 * @return size_t** inner table structure
 */
size_t** lock_table(CostTable* tbl) {
    pthread_mutex_lock(tbl->lock);
    return tbl->table;
}

/**
 * @brief Unlocks the table
 * 
 * @param tbl Cost table
 */
void unlock_table(CostTable* tbl) {
    pthread_mutex_unlock(tbl->lock);
}

/**
 * @brief Prints current neighbor costs
 * 
 * @param tbl CostTable
 */
void print_costs(CostTable* tbl) {
    size_t** table = lock_table(tbl);

    // print loop inside lock
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%zd\t", table[i][j]);
        }
        printf("\n");
    }

    unlock_table(tbl);
}

/**
 * @brief Updates the cost table with a new cost in both directions
 * 
 * @param tbl CostTable
 * @param msg Message from another node as [machine1, machine2, cost]
 */
void update_costs(CostTable* tbl, int* msg) {
    // reassign to variables to make more clear
    int x    = msg[0];
    int y    = msg[1];
    int cost = msg[2];

    size_t** table = lock_table(tbl);

    table[x][y] = cost;
    table[y][x] = cost;
    
    unlock_table(tbl);
}

/**
 * @brief Gets the index of closet machine
 * 
 * @param distances 
 * @param visited 
 * @return int 
 */
int get_closest(int* distances, int* visited) {
    int min = INT_MAX;
    int min_index;

    for (int i = 0; i < 4; ++i) {
        if (visited[i] == 0 && distances[i] <= min) {
            min = distances[i];
            min_index = i;
        }
    }

    return min_index;
}

/**
 * @brief Gets the least cost array
 * obtained with the link state algorithm using dijkstra's algorithm
 * 
 * @param tbl 
 * @param start 
 */
int* get_least_costs(CostTable* tbl, int start) {
    size_t** table = lock_table(tbl);

    // malloc since we need to use this after return
    int* distances = (int*) malloc(4 * sizeof(int));
    // initialize to max int
    for (int i = 0; i < 4; ++i) {
        distances[i] = INT_MAX;
    }

    // initialize to 0
    int visited[4] = {0};

    // start node always distance of 0 to itself
    distances[start] = 0;

    for (int i = 0; i < 4; ++i) {
        int min_index = get_closest(distances, visited);
        visited[min_index] = 1;

        for (int j = 0; j < 4; ++j) {
            if (!visited[j]            // have not visited
                && table[min_index][j] // is not zero
                && distances[min_index] < 10000 // is a neighbor
                // distance to closest + given node is closer than the given node
                && distances[min_index] + table[min_index][j] < distances[j]) {
                    distances[j] = distances[min_index] + table[min_index][j];
            }
        }
    }
    unlock_table(tbl);

    return distances;
}

/**
 * @brief Prints an array
 * 
 * @param arr  Array to print
 * @param size Size of array
 */
void print_array(int* arr, int size) {
    printf("[");

    for (int i = 0; i < size; ++i) {
        // last element with closing bracket
        if (i == size - 1) {
            printf("%d]\n", arr[i]);
            continue;
        }

        printf("%d, ", arr[i]);
    }
}
