//
// Created by nicho on 10/12/2022.
//

#ifndef MP1_CACHE_SET_H
#define MP1_CACHE_SET_H

#include <string>
#include <ostream>
#include "Block.h"

using namespace std;
class Set {
public:
    virtual void write(string cache_lvl, Block* block, unsigned int tag) = 0;
    virtual void read(string cache_lvl,Block* block, unsigned int tag) = 0;
    virtual Block* check_for_hit(string cache_lvl, unsigned int tag) = 0;
    virtual Block* select_victim() = 0;
    virtual Block* find_invalid_block(string cache_lvl) = 0;
    virtual Block* get_blocks() = 0;
    bool invalidate(int tag);

    friend ostream &operator<<(ostream &os, const Set &set);

protected:
    int assoc;
    int blocksize;
    Block* blocks;
};

#endif //MP1_CACHE_SET_H
