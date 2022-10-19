//
// Created by nicho on 10/18/2022.
//

#ifndef MP1_CACHE_OPTIMALSET_H
#define MP1_CACHE_OPTIMALSET_H

#include <vector>
#include "Set.h"

class OptimalSet: public Set {

public:
    explicit OptimalSet(int assoc);

    void write(string cache_lvl, Block *block, unsigned int tag) override;

    void read(string cache_lvl, Block *block, unsigned int tag) override;

    Block *check_for_hit(string cache_lvl, unsigned int tag) override;

    Block *select_victim() override;

    void add_to_vector(unsigned int tag);

private:
    vector<unsigned int> access_vector;
    int access_index;
};


#endif //MP1_CACHE_OPTIMALSET_H
