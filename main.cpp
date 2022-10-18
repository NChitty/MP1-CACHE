#include <string>
#include <iostream>
#include <fstream>
#include "Options.h"
#include "Cache.h"

using namespace std;

unsigned int conv_addr(string address);
void print_sim_results(Cache* l1, Cache* l2);

int hexMap[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
Options* options;
int main(int argc, char **argv) {
    options = new Options(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]), stoi(argv[4]), stoi(argv[5]), stoi(argv[6]), stoi(argv[7]), argv[8]);
    cout << *options;
    Cache* l1 = new Cache("L1", options->blocksize(), options->l1_size(), options->l1_assoc(), options->replacement_policy(), options->inclusion_property());
    Cache* l2 = nullptr;
    if (options->l2_size() != 0) {
        l2 = new Cache("L2", options->blocksize(), options->l2_size(), options->l2_assoc(),
                              options->replacement_policy(), options->inclusion_property());
        l2->set_prev_lvl(l1);
        l2->set_next_lvl(nullptr);
    }
    l1->set_next_lvl(l2);
    ifstream trace_file (options->trace_file());
    string current_line;
    if(trace_file.is_open()) {
        unsigned int mem_op = 1;
        while(trace_file) {
            getline (trace_file, current_line);
            if(current_line.empty()) break;
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
        cout << *l1;
        if(l2 != nullptr) cout << *l2;
        print_sim_results(l1, l2);
    }
    return 0;
}

unsigned int conv_addr(string addr) {
    unsigned int ret_addr = 0;
    int length = addr.length();
    for(int i = 0; i < 8 - length; i++) {
        ret_addr &= 0x0;
        ret_addr <<= 4;
    }
    for(int i = 0; i < length; i++) {
        ret_addr <<= 4;
        if(tolower(addr[i]) >= 'a' && tolower(addr[i]) <= 'f') {
            ret_addr |= hexMap[tolower(addr[i]) - 'a' + 10];
        } else {
            ret_addr |= hexMap[addr[i] - '0'];
        }
    }
    return ret_addr;
}

void print_sim_results(Cache* l1, Cache* l2) {
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
        l2_miss_rate =
                (double) l2->get_stats().read_misses / l2->get_stats().reads;
        cout << "g. number of L2 reads:         " << l2->get_stats().reads << endl;
        cout << "h. number of L2 read misses:   " << l2->get_stats().read_misses << endl;
        cout << "i. number of L2 writes:        " << l2->get_stats().writes << endl;
        cout << "j. number of L2 write misses:  " << l2->get_stats().write_misses << endl;
        printf("k. L2 miss rate:               %.6f\n", l2_miss_rate);
        cout << "l. number of L2 writebacks:    " << l2->get_stats().write_backs << endl;
        if(options->inclusion_property() == 0)
            cout << "m. total memory traffic:       "
                 << l2->get_stats().read_misses + l2->get_stats().write_misses + l2->get_stats().write_backs  << endl;
        else
            cout << "m. total memory traffic:       "
                 << l2->get_stats().read_misses + l2->get_stats().write_misses + l2->get_stats().write_backs << endl;
    } else {
        cout << "g. number of L2 reads:         " << 0 << endl;
        cout << "h. number of L2 read misses:   " << 0 << endl;
        cout << "i. number of L2 writes:        " << 0 << endl;
        cout << "j. number of L2 write misses:  " << 0 << endl;
        cout << "k. L2 miss rate:               " << 0 << endl;
        cout << "l. number of L2 writebacks:    " << 0 << endl;
        cout << "m. total memory traffic:       "
             << l1->get_stats().read_misses + l1->get_stats().write_misses + l1->get_stats().write_backs << endl;
    }
}
