//
// Created by nicho on 10/11/2022.
//

#ifndef MP1_CACHE_CACHE_H
#define MP1_CACHE_CACHE_H

#include <queue>
#include <list>
#include <cstdlib>
#include <ostream>
#include "structs/Block.h"
#include "sets/Set.h"
#include "structs/Stats.h"

#define LRU 0
#define FIFO 1
#define OPTIMAL 2

using namespace std;

class Cache {
public:
    Cache(string cache_lvl, int size, int blocksize, int assoc, int repl_policy, int incl_policy);
    void write(unsigned int address);
    void read(unsigned int address);
    void invalidate(unsigned int address);
    void set_next_lvl(Cache* l2);
    void set_prev_lvl(Cache* l1);
    static string to_hex(unsigned int val);

    friend ostream &operator<<(ostream &os, const Cache &cache);

private:
    Set** cache;
    int size;
    int blocksize;
    int assoc;
    int repl_policy;
    int incl_policy;
    unsigned int offset_mask = 0;
    unsigned int index_mask = 0;
    int offset_bits;
    int index_bits;
    string cache_lvl;
    Stats* stats;
    Cache* previous_lvl = nullptr;
    Cache* next_lvl = nullptr;

};

#endif //MP1_CACHE_CACHE_H
