//
// Created by nicho on 10/11/2022.
//
#include <cmath>
#include <iostream>
#include "Cache.h"
#include "LRUSet.h"
#include "FIFOSet.h"
#include "OptimalSet.h"

const char hex_map[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

Cache::Cache(const string& cache_lvl, int blocksize, int size, int assoc, int repl_policy, int incl_policy) : cache_lvl(cache_lvl), repl_policy(repl_policy), incl_policy(incl_policy) {
    stats = (Stats*) calloc(1, sizeof(Stats));
    int sets = size/(blocksize*assoc);
    switch(repl_policy) {
        case LRU:
            cache = (Set**) calloc(sets, sizeof(LRUSet*));
            for(int i = 0; i < sets; i++) {
                cache[i] = new LRUSet(assoc);
            }
            break;
        case FIFO:
            cache = (Set**) calloc(sets, sizeof(FIFOSet*));
            for(int i = 0; i < sets; i++) {
                cache[i] = new FIFOSet(assoc);
            }
            break;
        case OPTIMAL:
            cache = (Set**) calloc(sets, sizeof(OptimalSet*));
            for(int i = 0; i < sets; i++) {
                cache[i] = new OptimalSet(assoc);
            }
            break;
        default:
            break;
    }
    this->index_bits = log2(sets);
    this->offset_bits = log2(blocksize);
    this->index_mask = 0;
    this->offset_mask = 0;
    for(int i = 0; i < this->index_bits; i++) {
        this->index_mask <<= 1;
        this->index_mask |= 1;
    }
    for(int i = 0; i < offset_bits; i++) {
        this->offset_mask <<= 1;
        this->offset_mask |= 1;
    }
}

void Cache::write(unsigned int address) {
    // get offset, index and tag from address
    unsigned int offset, index, tag;
    this->decode_address(address, &tag, &index, &offset);
    unsigned int addr = (address >> this->offset_bits) << this->offset_bits;

    stats->writes++;

    cout << cache_lvl << " write : "<< to_hex(addr)
        << " (tag " << to_hex(tag) << ", index " << index << ")" << endl;

    Block* block = cache[index]->check_for_hit(cache_lvl, tag);

    if(block != nullptr) {
        cout << cache_lvl << " set dirty" << endl;
        block->dirty = true;
    } else {
        stats->write_misses++;

        Block* victim = cache[index]->find_invalid_block(cache_lvl);

        if(victim == nullptr) {
            victim = cache[index]->select_victim();

            cout << cache_lvl << " victim: " << to_hex(((((0 | victim->tag) << index_bits) | index)
                    << offset_bits)) << " (tag " << to_hex(victim->tag)
                 << ", index " << index << ", " << (victim->dirty ? "dirty)" : "clean)") << endl;

            if(victim->dirty) {
                stats->write_backs++;
                if(next_lvl != nullptr) {
                    unsigned int victim_addr;
                    this->encode_address(&victim_addr, victim->tag, index, offset);
                    next_lvl->write(victim_addr);
                }
            } else {
                victim->valid = false;
            }

            if(incl_policy == 1 && next_lvl == nullptr) {
                unsigned int victim_addr;
                this->encode_address(&victim_addr, victim->tag, index, offset);
                next_lvl->invalidate(victim_addr);
            }
        }

        if(next_lvl != nullptr) {
            next_lvl->read(addr + offset);
        }
        cache[index]->write(cache_lvl, victim, tag);
        cout << cache_lvl << " set dirty" << endl;
        victim->dirty = true;
    }
}

void Cache::read(unsigned int address) {
    unsigned int offset, index, tag;
    this->decode_address(address, &tag, &index, &offset);
    unsigned int addr = (address >> this->offset_bits) << this->offset_bits;

    cout << cache_lvl << " read : "<< to_hex(addr) << " (tag " << to_hex(tag) << ", index " << index << ")" << endl;

    stats->reads++;

    Block* block = cache[index]->check_for_hit(cache_lvl, tag);

    if (block == nullptr) {
        stats->read_misses++;
        Block *victim_block = cache[index]->find_invalid_block(cache_lvl);

        if (victim_block == nullptr) {
            victim_block = cache[index]->select_victim();

            cout << cache_lvl << " victim: " << to_hex(((((0 | victim_block->tag) << index_bits) | index)
                    << offset_bits)) << " (tag " << to_hex(victim_block->tag) << ", index "
                 << index << ", " << (victim_block->dirty ? "dirty)" : "clean)") << endl;

            if (victim_block->dirty) {
                stats->write_backs++;
                if (next_lvl != nullptr) {
                    unsigned int victim_addr;
                    this->encode_address(&victim_addr, victim_block->tag, index, offset);
                    next_lvl->write(victim_addr);
                }
                victim_block->dirty = false;
                victim_block->valid = false;
            } else {
                victim_block->valid = false;
            }

            if (incl_policy == 1 && next_lvl == nullptr) {
                unsigned int victim_addr;
                this->encode_address(&victim_addr, victim_block->tag, index, offset);
                previous_lvl->invalidate(victim_addr);
            }
        }

        if (next_lvl != nullptr) {
            next_lvl->read(addr  + offset);
        }
        cache[index]->read(cache_lvl, victim_block, tag);
    }
}

void Cache::decode_address(unsigned int address, unsigned int *tag, unsigned int *index, unsigned int *offset) {
    *offset = address & this->offset_mask;
    address >>= this->offset_bits;
    *index = (address & this->index_mask);
    address >>= this->index_bits;
    *tag = address;
}

void Cache::encode_address(unsigned int* address, unsigned int tag, unsigned int index, unsigned int offset) {
    *address = tag;
    *address <<= this->index_bits;
    *address |= index;
    *address <<= offset_bits;
    *address |= offset;
}

// TODO output invalidation in correct places
// TODO correct writeback total in l2 for incl = 1

void Cache::invalidate(unsigned int address) {
    unsigned int tag, index, offset;
    this->decode_address(address, &tag, &index, &offset);
    if(cache[index]->invalidate(tag)) {
        // write back to main
        if(next_lvl != nullptr) {
            cout << "L1 writeback to main memory directly" << endl;
        }
    }
}

Set* Cache::get_set(unsigned int index) {
    return this->cache[index];
}

void Cache::set_next_lvl(Cache *l2) {
    this->next_lvl = l2;
}

void Cache::set_prev_lvl(Cache *l1) {
    this->previous_lvl = l1;
}

Stats Cache::get_stats() {
    return *this->stats;
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
        os << "Set     "<< i << ": " << *(cache.cache[i]) << endl;
    }
    return os;
}


