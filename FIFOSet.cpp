//
// Created by nicho on 10/17/2022.
//

#include <iostream>
#include "FIFOSet.h"

using namespace std;

FIFOSet::FIFOSet(int assoc) {
    this->assoc = assoc;
    blocks = (Block*) calloc(assoc, sizeof(Block));
    for(int i = 0; i < this->assoc; i++) {
        blocks[i].valid = false;
        blocks[i].dirty = false;
    }
    this->fifo_val = 0;
}

void FIFOSet::write(string cache_lvl, Block *block, unsigned int tag) {
    cout << cache_lvl << " update FIFO" << endl;
    block->repl_val = ++this->fifo_val;

    block->valid = true;
    block->tag = tag;
}

void FIFOSet::read(string cache_lvl, Block *block, unsigned int tag) {
    if(tag != block->tag) {
        cout << cache_lvl << " update FIFO" << endl;
        block->repl_val = ++this->fifo_val;
    }
    block->valid = true;
    block->tag = tag;
}

Block *FIFOSet::check_for_hit(string cache_lvl, unsigned int tag) {
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].tag == tag) {
            cout << cache_lvl << " hit" << endl;
            // I do not understand why we would update fifo
            // on a hit but it works
            cout << cache_lvl << " update FIFO" << endl;
            // commented out updating the repl value
            return &(blocks[i]);
        }
    }
    cout << cache_lvl << " miss" << endl;
    return nullptr;
}

Block *FIFOSet::select_victim() {
    int lowest_fifo = this->fifo_val+1;
    int index = 0;
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].repl_val < lowest_fifo) {
            index = i;
            lowest_fifo = blocks[i].repl_val;
        }
    }
    return blocks + index;
}

Block *FIFOSet::find_invalid_block(string cache_lvl) {
    for(int i = 0; i < this->assoc; i++) {
        if(!blocks[i].valid) {
            cout << cache_lvl << " victim: none" << endl;
            return &(blocks[i]);
        }
    }
    return nullptr;
}
