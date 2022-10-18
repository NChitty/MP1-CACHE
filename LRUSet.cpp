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
    this->lru_val = 0;
}

void LRUSet::write(string cache_lvl, Block* victim, unsigned int tag) {
    cout << cache_lvl << " update LRU" << endl;

    victim->valid = true;
    victim->tag = tag;
    victim->repl_val = ++this->lru_val;
}

void LRUSet::read(string cache_lvl, Block* victim, unsigned int tag) {
    // update lru
    cout << cache_lvl << " update LRU" << endl;

    victim->valid = true;
    victim->tag = tag;
    victim->repl_val = ++this->lru_val;
}

Block *LRUSet::check_for_hit(string cache_lvl, unsigned int tag) {
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].tag == tag) {
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

Block *LRUSet::find_invalid_block(string cache_lvl) {
    for(int i = 0; i < this->assoc; i++) {
        if(!blocks[i].valid) {
            cout << cache_lvl << " victim: none" << endl;
            return &(blocks[i]);
        }
    }
    return nullptr;
}
