//
// Created by nicho on 10/17/2022.
//

#ifndef MP1_CACHE_FIFOSET_H
#define MP1_CACHE_FIFOSET_H


#include "Set.h"

class FIFOSet: public Set {
public:
    FIFOSet(int assoc);

    void write(string cache_lvl, Block* block, unsigned int tag) override;

    void read(string cache_lvl, Block* block, unsigned int tag) override;

    Block *check_for_hit(string cache_lvl, unsigned int tag) override;

    Block *select_victim() override;

    Block *find_invalid_block(string cache_lvl) override;

private:
    int fifo_val;


};


#endif //MP1_CACHE_FIFOSET_H
