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
    // from formula calculate number of sets
    int sets = size/(blocksize*assoc);
    switch(repl_policy) {
        // use abstraction to generate sets the implement the different replacement policies
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
    // build masks to encode and decode the memory address
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
    // decode tag, index, and offset
    unsigned int offset, index, tag;
    this->decode_address(address, &tag, &index, &offset);
    unsigned int addr = (address >> this->offset_bits) << this->offset_bits;

    // count every write
    stats->writes++;

    cout << cache_lvl << " write : "<< to_hex(addr)
        << " (tag " << to_hex(tag) << ", index " << index << ")" << endl;

    // get the pointer to the block in the set on a hit
    Block* block = cache[index]->check_for_hit(cache_lvl, tag);

    // if the check for hit returned a block
    if(block != nullptr) {
        // hit and set the dirty bit
        cout << cache_lvl << " set dirty" << endl;
        block->dirty = true;
    } else {
        // miss
        stats->write_misses++;

        // try to find a victim block by finding an invalid block in the set
        Block* victim = cache[index]->find_invalid_block(cache_lvl);

        if(victim == nullptr) {
            // we did not find an invalid block so select the victim based on the replacement policy
            victim = cache[index]->select_victim();

            cout << cache_lvl << " victim: " << to_hex(((((0 | victim->tag) << index_bits) | index)
                    << offset_bits)) << " (tag " << to_hex(victim->tag)
                 << ", index " << index << ", " << (victim->dirty ? "dirty)" : "clean)") << endl;

            if(victim->dirty) {
                // if the victim is dirty we need to perform a writeback
                stats->write_backs++;
                if(next_lvl != nullptr) {
                    // we have a higher level of cache so perform the writeback here
                    unsigned int victim_addr;
                    // reassemble the address
                    this->encode_address(&victim_addr, victim->tag, index, offset);
                    // perform the operation at the next level of cache
                    next_lvl->write(victim_addr);
                }
            } else {
                // the victim block was not dirty so we just have to invalidate it
                victim->valid = false;
            }

            // if this instance of cache is L2 and inclusive
            if(incl_policy == 1 && next_lvl == nullptr) {
                unsigned int victim_addr;
                // encode the victim's address and invalidate it in the previous level
                this->encode_address(&victim_addr, victim->tag, index, offset);
                previous_lvl->invalidate(victim_addr);
            }
        }

        // we missed a write so now we have to read from next level if it exists
        if(next_lvl != nullptr) {
            next_lvl->read(addr + offset);
        }
        // take what was read (there is assumed to be a main memory read if no next_lvl) and write it to the block
        cache[index]->write(cache_lvl, victim, tag);
        // set it as dirty
        cout << cache_lvl << " set dirty" << endl;
        victim->dirty = true;
    }
}

void Cache::read(unsigned int address) {
    // decode address
    unsigned int offset, index, tag;
    this->decode_address(address, &tag, &index, &offset);
    unsigned int addr = (address >> this->offset_bits) << this->offset_bits;

    cout << cache_lvl << " read : "<< to_hex(addr) << " (tag " << to_hex(tag) << ", index " << index << ")" << endl;

    // count every read
    stats->reads++;

    // check for a hit
    Block* block = cache[index]->check_for_hit(cache_lvl, tag);

    if (block == nullptr) {
        // miss
        stats->read_misses++;

        // try to find an invalid block to replace
        Block *victim_block = cache[index]->find_invalid_block(cache_lvl);

        if (victim_block == nullptr) {
            // we didn't find an invalid block so select the victim based on the replacement policy
            victim_block = cache[index]->select_victim();

            cout << cache_lvl << " victim: " << to_hex(((((0 | victim_block->tag) << index_bits) | index)
                    << offset_bits)) << " (tag " << to_hex(victim_block->tag) << ", index "
                 << index << ", " << (victim_block->dirty ? "dirty)" : "clean)") << endl;

            if (victim_block->dirty) {
                // the victim is dirty so we need to write back to the next level or main memory
                stats->write_backs++;
                if (next_lvl != nullptr) {
                    // encode address and perform the write to the next level of cache if it exists
                    unsigned int victim_addr;
                    this->encode_address(&victim_addr, victim_block->tag, index, offset);
                    next_lvl->write(victim_addr);
                }
                victim_block->dirty = false;
                victim_block->valid = false;
            } else {
                // it is not dirty but this block is no longer valid
                victim_block->valid = false;
            }

            // if inclusive and currently the bottom layer
            if (incl_policy == 1 && next_lvl == nullptr) {
                // encode address
                unsigned int victim_addr;
                this->encode_address(&victim_addr, victim_block->tag, index, offset);
                // invalidate the block at the previous level
                previous_lvl->invalidate(victim_addr);
            }
        }

        if (next_lvl != nullptr) {
            // if the l1 then try to get the block from l2
            next_lvl->read(addr  + offset);
        }
        // we have now read from the next lvl or main memory so we can read the block in cache
        cache[index]->read(cache_lvl, victim_block, tag);
    }
}

void Cache::decode_address(unsigned int address, unsigned int *tag, unsigned int *index, unsigned int *offset) const {
    // the LSB of address are offset so mask
    *offset = address & this->offset_mask;
    // and shift right to make the LSB the index
    address >>= this->offset_bits;
    // mask the index
    *index = (address & this->index_mask);
    // and shift right to make the remaining bits the tag
    address >>= this->index_bits;
    *tag = address;
}

void Cache::encode_address(unsigned int* address, unsigned int tag, unsigned int index, unsigned int offset) const {
    // set the address to the tag
    *address = tag;
    // make room for the index
    *address <<= this->index_bits;
    // set the index bits
    *address |= index;
    // make room for the offset
    *address <<= offset_bits;
    // set the offset bits
    *address |= offset;
}

// this function only gets used in inclusive policies
// I am not sure why but I could not debug why there are 3 missing writebacks from l2
void Cache::invalidate(unsigned int address) {
    unsigned int tag, index, offset;
    // decode the address
    this->decode_address(address, &tag, &index, &offset);
    if(cache[index]->invalidate(*this, tag, index)) { // if the block is dirty
        // write back to main mem
        if(next_lvl != nullptr) {
            next_lvl->stats->write_backs--; // l1 performed the writeback directly so this does not count
            // as a writeback from l2
            this->stats->write_to_mem++;
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
    // create a string buffer of all 0s
    string chars = "00000000";
    // set the index to the least significant hexadecimal
    int index = 7;
    while(val > 0) {
        // get the bottom 4 bits and map to a character in the buffer
        chars.begin()[index--] = hex_map[val % 16];
        // shift right 4 bits
        val /= 16;
    }
    // return the buffer excluding leading zeros
    return chars.substr(index+1);
}

// output
ostream &operator<<(ostream &os, const Cache &cache) {

    os << "===== " << cache.cache_lvl << " contents =====\n";
    for(int i = 0; i < pow(2, cache.index_bits); i++) {
        os << "Set     "<< i << ": " << *(cache.cache[i]) << endl;
    }
    return os;
}


