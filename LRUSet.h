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
    LRUSet(int assoc, int blocksize);

    void write(string cache_lvl, Block* block, unsigned int tag) override;

    void read(string cache_lvl, Block* block, unsigned int tag) override;

    Block *check_for_hit(string cache_lvl, unsigned int tag) override;

    Block *select_victim() override;

    Block *find_invalid_block(string cache_lvl) override;

    Block *get_blocks() override;

private:
    // stores the tag as the key and the index of the block in the list
    unordered_map<int, int> block_map;
    // stores the tags in the order of their usage
    list<int> lru_list;
};


#endif //MP1_CACHE_LRUSET_H
