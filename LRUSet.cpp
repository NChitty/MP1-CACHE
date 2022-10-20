//
// Created by nicho on 10/12/2022.
//

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include "LRUSet.h"

LRUSet::LRUSet(int assoc) {
    // general constructor but classes with virtual methods do not allow a constructor
    this->assoc = assoc;
    blocks = (Block*) calloc(assoc, sizeof(Block));
    for(int i = 0; i < this->assoc; i++) {
        blocks[i].valid = false;
        blocks[i].dirty = false;
    }
    this->lru_val = 0;
}

void LRUSet::write(string cache_lvl, Block* victim, unsigned int tag) {
    // this block was used more recently so set the block repl val to the incremented current lru val
    cout << cache_lvl << " update LRU" << endl;
    victim->repl_val = ++this->lru_val;

    victim->valid = true;
    victim->tag = tag;
}

void LRUSet::read(string cache_lvl, Block* victim, unsigned int tag) {
    // this block was used more recently so set the block repl val to the incremented current lru val
    cout << cache_lvl << " update LRU" << endl;
    victim->repl_val = ++this->lru_val;

    victim->valid = true;
    victim->tag = tag;
}

Block *LRUSet::check_for_hit(string cache_lvl, unsigned int tag) {
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].tag == tag && blocks[i].valid) {
            // this block was used in a hit so update the lru
            cout << cache_lvl << " hit" << endl;
            blocks[i].repl_val = ++this->lru_val;
            cout << cache_lvl << " update LRU" << endl;
            return &(blocks[i]);
        }
    }
    cout << cache_lvl << " miss" << endl;
    return nullptr;
}

Block *LRUSet::select_victim() {
    // find the lowest lru and return this as the victim
    int lowest_lru = this->lru_val+1;
    int index = 0;
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].repl_val < lowest_lru) {
            index = i;
            lowest_lru = blocks[i].repl_val;
        }
    }
    return blocks + index;
}

