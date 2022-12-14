//
// Created by nicho on 10/12/2022.
//

#ifndef MP1_CACHE_STATS_H
#define MP1_CACHE_STATS_H

/**
 * Simply a struct for keeping track of the simulation
 * results without bloating the Cache class
 */
typedef struct Stats {
    int reads;
    int read_misses;
    int writes;
    int write_misses;
    int write_backs;
    int write_to_mem;
}Stats;

#endif //MP1_CACHE_STATS_H
