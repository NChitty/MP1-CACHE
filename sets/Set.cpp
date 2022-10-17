//
// Created by nicho on 10/12/2022.
//
#include "Set.h"
#include "../Cache.h"

bool Set::invalidate(unsigned int tag) {
    int len = sizeof(this->blocks);
    for(int i = 0; i < len; i++) {
        if(tag == blocks[i].tag && blocks[i].valid) {
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
        os << Cache::to_hex(set.blocks[i].tag) << " " << (set.blocks[i].dirty ? "   " : "D  ");
    }
    return os;
}

