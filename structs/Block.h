//
// Created by nicho on 10/11/2022.
//

#ifndef MP1_CACHE_BLOCK_H
#define MP1_CACHE_BLOCK_H
typedef struct Block {
    unsigned int tag;
    bool dirty;
    bool valid;
}Block;
#endif //MP1_CACHE_BLOCK_H
