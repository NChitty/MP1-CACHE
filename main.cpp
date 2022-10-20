#include <string>
#include <iostream>
#include <fstream>
#include "Options.h"
#include "Cache.h"
#include "OptimalSet.h"

using namespace std;

unsigned int conv_addr(string address);
void print_sim_results(Cache* l1, Cache* l2);

int hexMap[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
Options* options;
int main(int argc, char **argv) {
    // read in arguments
    options = new Options(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]), stoi(argv[4]), stoi(argv[5]), stoi(argv[6]), stoi(argv[7]), argv[8]);
    // output simulation settings
    cout << *options;
    // create new l1 cache from options
    Cache* l1 = new Cache("L1", options->blocksize(), options->l1_size(), options->l1_assoc(), options->replacement_policy(), options->inclusion_property());
    Cache* l2 = nullptr;
    // if the settings permit, create l2 cache
    if (options->l2_size() != 0) {
        l2 = new Cache("L2", options->blocksize(), options->l2_size(), options->l2_assoc(),
                              options->replacement_policy(), options->inclusion_property());
        l2->set_prev_lvl(l1);
        l2->set_next_lvl(nullptr);
    }
    // set the next and previous levels of l1 appropriately
    l1->set_next_lvl(l2);
    l1->set_prev_lvl(nullptr);
    // open file
    ifstream trace_file (options->trace_file());
    string current_line;
    // preprocess
    if(options->replacement_policy() == 2) {
        if(trace_file.is_open()) {
            while(trace_file) {
                getline(trace_file, current_line);
                if (current_line.empty()) break;
                size_t index = current_line.find(' ');
                string address = current_line.substr(index + 1);
                unsigned int addr = conv_addr(address);
                // decode address
                unsigned int tag, set, offset;
                l1->decode_address(addr, &tag, &set, &offset);
                // add the tag to the end of the vector for this set
                ((OptimalSet*) l1->get_set(set))->add_to_vector(tag);
            }
        }
        // reset file input stream to beginning of file
        trace_file.clear();
        trace_file.seekg(0);
    }
    // process memory operations
    if(trace_file.is_open()) {
        // this is the value for output
        unsigned int mem_op = 1;
        while(trace_file) {
            getline (trace_file, current_line);
            if(current_line.empty()) break;
            // all this to get the type of memory operation and the address
            size_t index = current_line.find(' ');
            int read_write = current_line.substr(0, index) == "r" ? 0 : 1;
            string address = current_line.substr(index+1);
            unsigned int addr = conv_addr(address);
            if(read_write) {
                // write
                cout << "----------------------------------------\n"
                << "# " << mem_op++ << " : write " << address << "\n";
                l1->write(addr);
            } else {
                // read
                cout << "----------------------------------------\n"
                     << "# " << mem_op++ << " : read " << address << "\n";
                l1->read(addr);
            }
        }
        trace_file.clear();
        trace_file.seekg(0);
    }
    // output the l1 cache (which will also print the set cotents)
    cout << *l1;
    // if l2 is not null print it as well
    if(l2 != nullptr) cout << *l2;
    // get the stats from l1 and l2 and print
    print_sim_results(l1, l2);
    return 0;
}

unsigned int conv_addr(string addr) {
    // start at an address of zero
    unsigned int ret_addr = 0;
    int length = addr.length();
    // if the string length is less than 8 prepend 8-n zeros
    for(int i = 0; i < 8 - length; i++) {
        ret_addr &= 0x0;
        ret_addr <<= 4;
    }
    for(int i = 0; i < length; i++) {
        // left shift 4 bits to move out previous 4 bits
        ret_addr <<= 4;
        if(tolower(addr[i]) >= 'a' && tolower(addr[i]) <= 'f') {
            // set 4 bits based on character as letter in hex
            ret_addr |= hexMap[tolower(addr[i]) - 'a' + 10];
        } else {
            // set 4 bits based on character but it is number
            ret_addr |= hexMap[addr[i] - '0'];
        }
    }
    return ret_addr;
}

void print_sim_results(Cache* l1, Cache* l2) {
    // calculate miss rate
    double l1_miss_rate =
            ((double) l1->get_stats().read_misses + l1->get_stats().write_misses) / (l1->get_stats().reads + l1->get_stats().writes);
    double l2_miss_rate;
    cout << "===== Simulation Results (raw) =====" << endl;
    cout << "a. number of L1 reads:         " << l1->get_stats().reads << endl;
    cout << "b. number of L1 read misses:   " << l1->get_stats().read_misses << endl;
    cout << "c. number of L1 writes:        " << l1->get_stats().writes << endl;
    cout << "d. number of L1 write misses:  " << l1->get_stats().write_misses << endl;
    printf("e. L1 miss rate:               %.6f\n", l1_miss_rate);
    cout << "f. number of L1 writebacks:    " << l1->get_stats().write_backs << endl;
    if(l2 != nullptr) {
        // calculate miss rate
        l2_miss_rate =
                (double) l2->get_stats().read_misses / l2->get_stats().reads;
        cout << "g. number of L2 reads:         " << l2->get_stats().reads << endl;
        cout << "h. number of L2 read misses:   " << l2->get_stats().read_misses << endl;
        cout << "i. number of L2 writes:        " << l2->get_stats().writes << endl;
        cout << "j. number of L2 write misses:  " << l2->get_stats().write_misses << endl;
        printf("k. L2 miss rate:               %.6f\n", l2_miss_rate);
        cout << "l. number of L2 writebacks:    " << l2->get_stats().write_backs << endl;
        // set memory traffic based solely on inclusion property per document
        if(options->inclusion_property() == 0)
            cout << "m. total memory traffic:       "
                 << l2->get_stats().read_misses + l2->get_stats().write_misses + l2->get_stats().write_backs  << endl;
        else
            cout << "m. total memory traffic:       "
                 << l2->get_stats().read_misses + l2->get_stats().write_misses + l2->get_stats().write_backs + l1->get_stats().write_to_mem << endl;
    } else {
        cout << "g. number of L2 reads:         " << 0 << endl;
        cout << "h. number of L2 read misses:   " << 0 << endl;
        cout << "i. number of L2 writes:        " << 0 << endl;
        cout << "j. number of L2 write misses:  " << 0 << endl;
        cout << "k. L2 miss rate:               " << 0 << endl;
        cout << "l. number of L2 writebacks:    " << 0 << endl;
        // set total memory traffic on l1 cache alone per document
        cout << "m. total memory traffic:       "
             << l1->get_stats().read_misses + l1->get_stats().write_misses + l1->get_stats().write_backs << endl;
    }
}
