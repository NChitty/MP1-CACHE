//
// Created by nicho on 10/17/2022.
//

#include <iostream>
#include "FIFOSet.h"

using namespace std;

FIFOSet::FIFOSet(int assoc) {
    // set the associativity
    this->assoc = assoc;
    // allocate space for all the blocks
    blocks = (Block*) calloc(assoc, sizeof(Block));
    for(int i = 0; i < this->assoc; i++) {
        // set the default/starting values of the valid and dirty bits
        blocks[i].valid = false;
        blocks[i].dirty = false;
    }
    // set the replacement value to 0
    this->fifo_val = 0;
}

void FIFOSet::write(string cache_lvl, Block *block, unsigned int tag) {
    cout << cache_lvl << " update FIFO" << endl;
    // increment the fifo counter
    block->repl_val = ++this->fifo_val;

    // the block is valid and gets the new tag
    block->valid = true;
    block->tag = tag;
}

void FIFOSet::read(string cache_lvl, Block *block, unsigned int tag) {
    if(tag != block->tag) {
        // if this read overwrote data due to a miss update fifo since the data is new
        cout << cache_lvl << " update FIFO" << endl;
        block->repl_val = ++this->fifo_val;
    }
    block->valid = true;
    block->tag = tag;
}

Block *FIFOSet::check_for_hit(string cache_lvl, unsigned int tag) {
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].tag == tag && blocks[i].valid) {
            cout << cache_lvl << " hit" << endl;
            // I do not understand why we would update fifo
            // as this means the data already existed properly in cache
            cout << cache_lvl << " update FIFO" << endl;
            // the above line is for output validation but again, we shouldn't update
            // fifo value as the block already existed
            return &(blocks[i]);
        }
    }
    cout << cache_lvl << " miss" << endl;
    return nullptr;
}

Block *FIFOSet::select_victim() {
    // find the lowest fifo val in this set and select this as the victim
    int lowest_fifo = this->fifo_val+1;
    int index = 0;
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].repl_val < lowest_fifo) {
            index = i;
            lowest_fifo = blocks[i].repl_val;
        }
    }
    // if no victim was found return the leftmost block
    return blocks + index;
}
