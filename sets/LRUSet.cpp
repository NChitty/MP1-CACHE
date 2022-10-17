//
// Created by nicho on 10/12/2022.
//

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include "LRUSet.h"

LRUSet::LRUSet(int assoc) {
    this->assoc = assoc;
    blocks = (Block*) calloc(assoc, sizeof(Block));
    for(int i = 0; i < this->assoc; i++) {
        blocks[i].valid = false;
        blocks[i].dirty = false;
    }
}

void LRUSet::write(string cache_lvl, Block* victim, unsigned int tag) {
    // update lru
    cout << cache_lvl << " update LRU" << endl;
    lru_list.push_back(tag);
    int index = block_map[victim->tag];
    block_map.erase(victim->tag);

    victim->valid = true;
    victim->tag = tag;
    block_map[victim->tag] = index;
}

void LRUSet::read(string cache_lvl, Block* victim, unsigned int tag) {
    // update lru
    cout << cache_lvl << " update LRU" << endl;
    lru_list.push_back(tag);
    int index = block_map[victim->tag];
    block_map.erase(victim->tag);

    victim->valid = true;
    victim->tag = tag;
    block_map[victim->tag] = index;
}

Block *LRUSet::check_for_hit(string cache_lvl, unsigned int tag) {
    if(find(lru_list.begin(), lru_list.end(), tag) != lru_list.end() && blocks[block_map[tag]].valid) {
        //hit. update lru
        cout << cache_lvl << " hit" << endl;
        remove(lru_list.begin(), lru_list.end(), tag);
        lru_list.push_back(tag);
        cout << cache_lvl << " update LRU" << endl;
        return blocks + block_map[tag];
    }
    cout << cache_lvl << " miss" << endl;
    return nullptr;
}

Block *LRUSet::select_victim() {
    if(!lru_list.empty() && !block_map.empty()) {
        int tag = lru_list.front();
        lru_list.pop_front();
        return blocks + block_map[tag];
    }
    return blocks;
}

Block *LRUSet::find_invalid_block(string cache_lvl) {
    for(int i = 0; i < this->assoc; i++) {
        if(!blocks[i].valid) {
            cout << cache_lvl << " victim: none" << endl;
            return blocks + i;
        }
    }
    return nullptr;
}

Block *LRUSet::get_blocks() {
    return blocks;
}
