//
// Created by nicho on 10/11/2022.
//
#include <cmath>
#include "Cache.h"

Cache::Cache(int blocksize, int size, int assoc, int repl_policy) : blocksize(blocksize), size(size), assoc(assoc), repl_policy(repl_policy){
    int sets = size/(blocksize*assoc);
    if(assoc == 1) {
        sets = 1;
        cache = (Block **) calloc(sets, sizeof(Block *));
        cache[1] = (Block *) calloc(size / blocksize, sizeof(Block));
        for(int j = 0; j < size/blocksize; j++) {
            cache[1][j].size = blocksize;
            cache[1][j].valid = false;
            cache[1][j].dirty = false;
        }
        index_mask = 0xFFFFFFFFFFFFFFFF;
        offset_bits = log2(blocksize);
        for(int i = 0; i < offset_bits; i++) {
            offset_mask |= 1;
            offset_mask <<= 1;
        }
        for(int i = 0; i < 32 - offset_bits; i++) {
            tag_mask |= 1;
            tag_mask <<= 1;
        }
    } else {
        cache = (Block **) calloc(sets, sizeof(Block *));
        for (int i = 0; i < sets; i++) {
            cache[i] = (Block *) calloc(assoc, sizeof(Block));
            for (int j = 0; j < assoc; j++) {
                cache[i][j].size = blocksize;
                cache[i][j].valid = false;
                cache[i][j].dirty = false;
            }
        }
        index_bits = log2(sets) + 1;
        offset_bits = log2(blocksize) + 1;
        for(int i = 0; i < this->index_bits; i++) {
            index_mask |= 1;
            index_mask <<= 1;
        }
        for(int i = 0; i < offset_bits; i++) {
            offset_mask |= 1;
            offset_mask <<= 1;
        }
        for(int i = 0; i < 32 - offset_bits - index_bits; i++) {
            tag_mask |= 1;
            tag_mask <<= 1;
        }
    }
}

void Cache::write(int address) {
    int index = 1;
    if(assoc != 1) {
        index = (address & this->index_mask) >> this->index_bits;
    }
    int tag = (address & this->tag_mask) >> (32 - this->offset_bits - this->index_bits);

    Block* block = this->checkForHit(index, tag);
    if(block != nullptr) {
        // write hit
        block->dirty = true;
    } else {
        // write miss

    }
}

Block* Cache::checkForHit(int set, int tag) {
    int len = assoc == 1 ? size/blocksize : assoc;
    for(int i = 0; i < len; i++) {
        if(cache[set][i].tag == tag && cache[set][i].valid) {
            switch(repl_policy) {
                case 0:
                    this->repl.push_back(tag << index_bits | set);
            }
            return cache[set] + i;
        }
    }

    return nullptr;
}

