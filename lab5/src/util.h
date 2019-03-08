#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Generates a random number between given bounds (inclusive).
 * On first run, it will seed the number generator.
 * 
 * @param low     lower bound
 * @param high    upper bound
 * @return size_t number between low and high
 */
size_t rand_range(size_t low, size_t high) {
    // persists between function calls
    static int initialized = 0;

    // call srand only first time
    if (!initialized) {
        srand(time(0)); 
        initialized = 1;
    }

    return rand() % (high + 1 - low) + low;
}

#endif