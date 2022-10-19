//
// Created by nicho on 10/12/2022.
//

#ifndef MP1_CACHE_LRUSET_H
#define MP1_CACHE_LRUSET_H


#include <unordered_map>
#include <list>
#include "Set.h"
#include "Cache.h"

using namespace std;

class LRUSet: public Set {
public:
    LRUSet(int assoc);

    void write(string cache_lvl, Block* block, unsigned int tag) override;

    void read(string cache_lvl, Block* block, unsigned int tag) override;

    Block *check_for_hit(string cache_lvl, unsigned int tag) override;

    Block *select_victim() override;

private:
    int lru_val;
};


#endif //MP1_CACHE_LRUSET_H
