//
// Created by nicho on 10/18/2022.
//

#include <iostream>
#include "OptimalSet.h"

using namespace std;

OptimalSet::OptimalSet(int assoc) {
    // boilerplate constructor, see FIFOSet for more details
    this->assoc = assoc;
    blocks = (Block*) calloc(assoc, sizeof(Block));
    for(int i = 0; i < this->assoc; i++) {
        blocks[i].valid = false;
        blocks[i].dirty = false;
    }
    this->access_index = 0;
}

void OptimalSet::write(string cache_lvl, Block *block, unsigned int tag) {
    // block valid and set tag
    block->valid = true;
    block->tag = tag;

    // update repl_val on block based on the policy
    cout << cache_lvl << " update optimal" << endl;
    // increment the index in the vector we are on
    this->access_index++;
    // use i as the value for the access_index at the next usage
    int i;
    for(i = this->access_index; i < this->access_vector.size(); i++) {
        if(this->access_vector[i] == tag) { // we found the next access
            block->repl_val = i; // set the block replacement value to the index of this next access
            break;
        }
    }
    if(i == this->access_vector.size()) // if we made it to the end of the vector
        block->repl_val = -1; // not being used again in this set so set it to a sentinel value

}

void OptimalSet::read(string cache_lvl, Block *block, unsigned int tag) {
    block->valid = true;
    block->tag = tag;

    cout << cache_lvl << " update optimal" << endl;
    this->access_index++; // increment the access that we are on
    int i; // use i to set repl_val to
    for(i = this->access_index; i < this->access_vector.size(); i++) { // loop through remaining accesses
        if(this->access_vector[i] == tag) { // if the tag at the index is equal to the tag then update the replacement
            block->repl_val = i;
            break; // stop looping, we found the next access for this tag in this set
        }
    }
    if(i == this->access_vector.size()) // if we made it to the end without breaking
        block->repl_val = -1; // set to sentinel to know that it is not being used again
}

Block *OptimalSet::check_for_hit(string cache_lvl, unsigned int tag) {
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].tag == tag && blocks[i].valid) {
            cout << cache_lvl << " hit" << endl;
            // if it is a cache hit we need to update the repl_val as before
            cout << cache_lvl << " update optimal" << endl;
            this->access_index++;
            int j;
            for(j = this->access_index; j < this->access_vector.size(); j++) {
                if(this->access_vector[j] == tag) {
                    blocks[i].repl_val = j;
                    break;
                }
            }
            if(j == this->access_vector.size())
                blocks[i].repl_val = -1;
            return &(blocks[i]);
        }
    }
    cout << cache_lvl << " miss" << endl;
    return nullptr;
}

Block *OptimalSet::select_victim() {
    // get the largest value
    int farthest = -2;
    int index = 0; // this will be the index of the victim in this set
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].repl_val == -1) { // we found a block that is not being used again, return immediately
            return &(blocks[i]);
        }
        if(blocks[i].repl_val > farthest) { // the replacement is value of this block is farther than the current farthest
            farthest = blocks[i].repl_val; // update farthest
            index = i; // update the index
        }
    }
    return blocks + index;
}

void OptimalSet::add_to_vector(unsigned int tag) {
    access_vector.push_back(tag);
}


