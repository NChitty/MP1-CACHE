//
// Created by nicho on 10/11/2022.
//

#ifndef MP1_CACHE_CACHE_H
#define MP1_CACHE_CACHE_H

#include <queue>
#include <list>
#include <cstdlib>
#include "Block.h"

using namespace std;

class Cache {
public:
    Cache(int size, int blocksize, int assoc, int repl_policy);
    void write(int address);
    Block* checkForHit(int set, int tag);

private:
    Block** cache;
    int size;
    int blocksize;
    int assoc;
    int repl_policy;
    int offset_mask;
    int index_mask;
    int tag_mask;
    list<int> repl;
    int offset_bits;
    int index_bits;


};

#endif //MP1_CACHE_CACHE_H
