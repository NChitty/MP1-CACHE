//
// Created by nicho on 10/3/2022.
//

#ifndef MP1_CACHE_OPTIONS_H
#define MP1_CACHE_OPTIONS_H


#include <string>
#include <ostream>

class Options {
public:
    friend std::ostream &operator<<(std::ostream &os, const Options &options) {
        os << "===== Simulator configuration =====\n"
           << "BLOCKSIZE:             " << options.blocksize_ << "\n"
           << "L1_SIZE:               " << options.l1_size_ << "\n"
           << "L1_ASSOC:              " << options.l1_assoc_ << "\n"
           << "L2_SIZE:               " << options.l2_size_ << "\n"
           << "L2_ASSOC:              " << options.l2_assoc_ << "\n"
           << "REPLACEMENT POLICY:    " << options.replacement_policy_str() << "\n"
           << "INCLUSION PROPERTY:    " << options.inclusion_property_str() << "\n"
        << "trace_file:            " << options.trace_file_ << "\n";
        return os;
    }

public:
    Options(const int blocksize,
            const int l1_size,
            const int l1_assoc,
            const int l2_size,
            const int l2_assoc,
            const int replacement_policy,
            const int inclusion_property,
            const std::string trace_file):
                blocksize_(blocksize),
                l1_size_(l1_size),
                l1_assoc_(l1_assoc),
                l2_size_(l2_size),
                l2_assoc_(l2_assoc),
                replacement_policy_(replacement_policy),
                inclusion_property_(inclusion_property),
                trace_file_(trace_file)
    {
    }

private:
    int blocksize_;
    int l1_size_;
    int l1_assoc_;
    int l2_size_;
    int l2_assoc_;
    int replacement_policy_;
    int inclusion_property_;
    std::string trace_file_;

public:
    int blocksize() const {
        return blocksize_;
    }

    int l1_size() const {
        return l1_size_;
    }

    int l1_assoc() const {
        return l1_assoc_;
    }

    int l2_size() const {
        return l2_size_;
    }

    int l2_assoc() const {
        return l2_assoc_;
    }

    std::string replacement_policy_str() const {
        switch(replacement_policy_) {
            case 0:
                return "LRU";
            case 1:
                return "FIFO";
            case 2:
                return "Optimal";
            default:
                return "ERROR";
        }
    }

    std::string inclusion_property_str() const {
        if(inclusion_property_)
            return "inclusive";
        return "non-inclusive";
    }

    int replacement_policy() const {
        return replacement_policy_;
    }

    int inclusion_property() const {
        return inclusion_property_;
    }

    std::string trace_file() const {
        return trace_file_;
    }
};




#endif //MP1_CACHE_OPTIONS_H
