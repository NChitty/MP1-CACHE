//
// Created by nicho on 10/18/2022.
//

#include <iostream>
#include "OptimalSet.h"

using namespace std;

OptimalSet::OptimalSet(int assoc) {
    this->assoc = assoc;
    blocks = (Block*) calloc(assoc, sizeof(Block));
    for(int i = 0; i < this->assoc; i++) {
        blocks[i].valid = false;
        blocks[i].dirty = false;
    }
    this->access_index = 0;
}

void OptimalSet::write(string cache_lvl, Block *block, unsigned int tag) {
    block->valid = true;
    block->tag = tag;

    cout << cache_lvl << " update optimal" << endl;
    this->access_index++;
    int i;
    for(i = this->access_index; i < this->access_vector.size(); i++) {
        if(this->access_vector[i] == tag) {
            block->repl_val = i;
            break;
        }
    }
    if(i == this->access_vector.size())
        block->repl_val = -1;

}

void OptimalSet::read(string cache_lvl, Block *block, unsigned int tag) {
    block->valid = true;
    block->tag = tag;

    cout << cache_lvl << " update optimal" << endl;
    this->access_index++;
    int i;
    for(i = this->access_index; i < this->access_vector.size(); i++) {
        if(this->access_vector[i] == tag) {
            block->repl_val = i;
            break;
        }
    }
    if(i == this->access_vector.size())
        block->repl_val = -1;
}

Block *OptimalSet::check_for_hit(string cache_lvl, unsigned int tag) {
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].tag == tag && blocks[i].valid) {
            cout << cache_lvl << " hit" << endl;
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
    int farthest = -2;
    int index = 0;
    for(int i = 0; i < this->assoc; i++) {
        if(blocks[i].repl_val == -1) {
            return &(blocks[i]);
        }
        if(blocks[i].repl_val > farthest) {
            farthest = blocks[i].repl_val;
            index = i;
        }
    }
    return blocks + index;
}

void OptimalSet::add_to_vector(unsigned int tag) {
    access_vector.push_back(tag);
}


