//
// Created by nicho on 10/12/2022.
//
#include <iostream>
#include "Set.h"
#include "Cache.h"

using namespace std;

bool Set::invalidate(Cache cache, unsigned int tag, unsigned int index) {
    // this only gets called by l1 cache and with an inclusive property
    int len = sizeof(this->blocks); // get the number of blocks in the set
    for(int i = 0; i < len; i++) { // for each block
        if(tag == blocks[i].tag && blocks[i].valid) { // if the tag matches the tag
            unsigned int address; //encode the address for output
            cache.encode_address(&address, tag, index, 0);
            cout << "L1 invalidated: " << Cache::to_hex(address)
                 << "(tag " << Cache::to_hex(tag) << ", index " << index << ", "
                 << (blocks[i].dirty ? "dirty)" : "clean)") << endl;
            // invalidate it
            blocks[i].valid = false;
            bool ret = blocks[i].dirty;
            blocks[i].dirty = false;
            // tell the cache whether it was dirty or not so it can perform the writeback
            return ret;
        }
    }
    return false;
}

ostream &operator<<(ostream &os, const Set &set) {
    // how we output a set
    for(int i = 0; i < set.assoc; i++) {
        os << Cache::to_hex(set.blocks[i].tag) << " " << (set.blocks[i].dirty ? "D  " : "   ");
    }
    return os;
}

Block *Set::find_invalid_block(string cache_lvl) {
    // search for an invalid block
    for(int i = 0; i < this->assoc; i++) {
        if(!blocks[i].valid) {
            // invalid block found so there is no "true" victim per the replacement policy
            cout << cache_lvl << " victim: none" << endl;
            return &(blocks[i]);
        }
    }
    return nullptr;
}

