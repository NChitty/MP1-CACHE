//
// Created by nicho on 10/12/2022.
//
#include <iostream>
#include "Set.h"
#include "Cache.h"

using namespace std;

bool Set::invalidate(Cache cache, unsigned int tag, unsigned int index) {
    int len = sizeof(this->blocks);
    for(int i = 0; i < len; i++) {
        if(tag == blocks[i].tag && blocks[i].valid) {
            unsigned int address;
            cache.encode_address(&address, tag, index, 0);
            cout << "L1 invalidated: " << Cache::to_hex(address)
                 << "(tag " << Cache::to_hex(tag) << ", index " << index << ", "
                 << (blocks[i].dirty ? "dirty)" : "clean)") << endl;
            blocks[i].valid = false;
            bool ret = blocks[i].dirty;
            blocks[i].dirty = false;
            return ret;
        }
    }
    return false;
}

ostream &operator<<(ostream &os, const Set &set) {
    for(int i = 0; i < set.assoc; i++) {
        os << Cache::to_hex(set.blocks[i].tag) << " " << (set.blocks[i].dirty ? "D  " : "   ");
    }
    return os;
}

Block *Set::find_invalid_block(string cache_lvl) {
    for(int i = 0; i < this->assoc; i++) {
        if(!blocks[i].valid) {
            cout << cache_lvl << " victim: none" << endl;
            return &(blocks[i]);
        }
    }
    return nullptr;
}

