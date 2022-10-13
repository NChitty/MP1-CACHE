//
// Created by nicho on 10/11/2022.
//
#include <cmath>
#include <iostream>
#include "Cache.h"
#include "LRUSet.h"

const char hex_map[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

Cache::Cache(string cache_lvl, int blocksize, int size, int assoc, int repl_policy, int incl_policy) : cache_lvl(cache_lvl), blocksize(blocksize), size(size), assoc(assoc), repl_policy(repl_policy), incl_policy(incl_policy) {
    stats = (Stats*) calloc(1, sizeof(stats));
    int sets = size/(blocksize*assoc);
    if(assoc == 1) {
        sets = 1;
        switch(repl_policy) {
            case LRU:
                cache = (Set**) calloc(1, sizeof(LRUSet*));
                cache[0] = new LRUSet(size/blocksize, blocksize);
                break;
        }
        index_mask = 0x0;
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
        switch(repl_policy) {
            case LRU:
                cache = (Set**) calloc(sets, sizeof(LRUSet*));
                for(int i = 0; i < sets; i++) {
                    cache[i] = new LRUSet(assoc, blocksize);
                }
        }
        index_bits = log2(sets);
        offset_bits = log2(blocksize);
        for(int i = 0; i < this->index_bits; i++) {
            index_mask <<= 1;
            index_mask |= 1;
        }
        for(int i = 0; i < offset_bits; i++) {
            offset_mask <<= 1;
            offset_mask |= 1;
        }
        for(int i = 0; i < 32 - offset_bits - index_bits; i++) {
            tag_mask |= 1;
            tag_mask <<= 1;
        }
    }
}

void Cache::write(unsigned int address) {
    unsigned int index = 0;
    unsigned int addr = address;
    unsigned int offset = address & this->offset_mask;
    address >>= this->offset_bits;
    if(assoc != 1) {
        index = (address & this->index_mask);
        address >>= this->index_bits;
    }
    unsigned int tag = (address & this->tag_mask);

    cout << cache_lvl << " write : "<< to_hex(addr) << " (tag " << to_hex(tag) << ", index " << index << ")\n";

    stats->writes++;

    Block* block = cache[index]->check_for_hit(cache_lvl, tag);

    if(block != nullptr) {
        // hit
        block->dirty = true;
    } else {
        // miss
        stats->write_misses++;

        // check for a block that is invalid
        Block* victim_block = cache[index]->find_invalid_block(cache_lvl);
        // no victim block
        if(victim_block == nullptr) {
            // set selects victim based on replacement policy
            victim_block = cache[index]->select_victim();

            cout << cache_lvl << " victim: " << to_hex(((((0 | victim_block->tag) << index_bits) | index)
                    << offset_bits) + offset) << " (tag " << to_hex(victim_block->tag) << ", index " << index << ", " << (victim_block->dirty ? "dirty)\n" : "clean)\n");
            // if the victim block is dirty, write back
            // to next level of cache
            if(victim_block->dirty) {
                stats->write_backs++;

                // write in next level
                if(next_lvl != nullptr) {
                    unsigned int victim_addr =
                            ((((0 | victim_block->tag) << (32 - next_lvl->index_bits - next_lvl->offset_bits)) | index) << next_lvl->index_bits)
                            + offset;
                    next_lvl->write(victim_addr);
                }

                victim_block->dirty = false;
                victim_block->valid = false;
            } else {
                // not dirty so we can just invalidate it
                victim_block->valid = false;
            }
            // if cache is inclusive this is the bottom level
            if(incl_policy == 1 && next_lvl == nullptr) {
                unsigned int victim_addr =
                        (((0 | victim_block->tag) << (32 - previous_lvl->index_bits - previous_lvl->offset_bits) | index) << previous_lvl->index_bits)
                        + offset;
                // invalidate in the previous level
                previous_lvl->invalidate(victim_addr);
            }
        }

        if (next_lvl != nullptr) {
            if (repl_policy == 2) {

            } else {
                next_lvl->read(address);
            }
        }

        cache[index]->write(cache_lvl, victim_block, tag);
        cout << cache_lvl <<" set dirty\n";
        victim_block->dirty = true;
    }
}

void Cache::read(unsigned int address) {
    unsigned int index = 0;
    unsigned int addr = address;
    unsigned int offset = address & this->offset_mask;
    address >>= this->offset_bits;
    if(assoc != 1) {
        index = (address & this->index_mask);
        address >>= this->index_bits;
    }
    unsigned int tag = (address & this->tag_mask);

    cout << cache_lvl << " read : "<< to_hex(addr) << " (tag " << to_hex(tag) << ", index " << index << ")\n";

    Block* block = cache[index]->check_for_hit(cache_lvl, tag);

    if (block == nullptr) {
        stats->read_misses++;
        Block *victim_block = cache[index]->select_victim();

        cout << cache_lvl << " victim: " << to_hex(((((0 | victim_block->tag) << index_bits) | index)
                << offset_bits) + offset) << " (tag " << to_hex(victim_block->tag) << ", index " << index << ", " << (victim_block->dirty ? "dirty)\n" : "clean)\n");

        if (victim_block->dirty) {
            stats->write_backs++;
            if (next_lvl != nullptr) {
                unsigned int victim_addr =
                        ((((0 | victim_block->tag) << (32 - next_lvl->index_bits - next_lvl->offset_bits)) | index)
                                << next_lvl->index_bits)
                        + offset;
                next_lvl->write(victim_addr);
            }
            victim_block->dirty = false;
            victim_block->valid = false;
        } else {
            victim_block->valid = false;
        }

        if (incl_policy == 1 && next_lvl == nullptr) {
            unsigned int victim_addr =
                    (((0 | victim_block->tag) << (32 - previous_lvl->index_bits - previous_lvl->offset_bits) | index)
                            << previous_lvl->index_bits)
                    + offset;
            previous_lvl->invalidate(victim_addr);
        }

        if (next_lvl != nullptr) {
            if (repl_policy == 2) {

            } else {
                next_lvl->read(address);
            }
        }
        cache[index]->read(cache_lvl, victim_block, tag);
    }
}

void Cache::invalidate(unsigned int address) {
    unsigned int index = 0;
    unsigned int addr = address;
    unsigned int offset = address & this->offset_mask;
    address >>= this->offset_bits;
    if(assoc != 1) {
        index = (address & this->index_mask);
        address >>= this->index_bits;
    }
    unsigned int tag = (address & this->tag_mask);

    if(cache[index]->invalidate(tag)) {
        stats->write_back_to_main_mem++;
    }
}

void Cache::set_next_lvl(Cache *l2) {
    this->next_lvl = l2;
}

void Cache::set_prev_lvl(Cache *l1) {
    this->previous_lvl = l1;
}

 string Cache::to_hex(unsigned int val) {
    string chars = "00000000";
    int index = 7;
    while(val > 0) {
        chars.begin()[index--] = hex_map[val % 16];
        val /= 16;
    }
    return chars.substr(index+1);
}

ostream &operator<<(ostream &os, const Cache &cache) {
    os << "===== " << cache.cache_lvl << " contents =====\n";
    for(int i = 0; i < pow(2, cache.index_bits); i++) {
        os << "Set     "<< i << ": " << cache.cache[i];
    }
    return os;
}


