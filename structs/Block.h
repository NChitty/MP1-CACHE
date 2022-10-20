//
// Created by nicho on 10/11/2022.
//

#ifndef MP1_CACHE_BLOCK_H
#define MP1_CACHE_BLOCK_H
/**
 * This structure defines a block in a set.
 * The tag is the tag of the block
 * the dirty and valid bits are for tracking these items
 * The repl_val is an integer value where the lowest value always gets
 * replaced, the abstraction of the set determines how to set the repl_val.
 */
typedef struct Block {
    unsigned int tag;
    bool dirty;
    bool valid;
    int repl_val;
}Block;
#endif //MP1_CACHE_BLOCK_H
