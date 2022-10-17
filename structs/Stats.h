//
// Created by nicho on 10/12/2022.
//

#ifndef MP1_CACHE_STATS_H
#define MP1_CACHE_STATS_H

typedef struct Stats {
    int reads = 0;
    int read_misses = 0;
    int writes = 0;
    int write_misses = 0;
    int write_backs = 0;
    int write_back_to_main_mem = 0;
}Stats;

#endif //MP1_CACHE_STATS_H
