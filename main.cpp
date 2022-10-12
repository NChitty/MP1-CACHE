#include <string>
#include <iostream>
#include <fstream>
#include "Options.h"

using namespace std;

int conv_addr(string address);

int hexMap[] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
Options* options;
int main(int argc, char **argv) {
    options = new Options(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]), stoi(argv[4]), stoi(argv[5]), stoi(argv[6]), stoi(argv[7]), argv[8]);
    cout << *options;
    ifstream trace_file (options->trace_file());
    string current_line;
    if(trace_file.is_open()) {
        while(trace_file) {
            getline (trace_file, current_line);
            int index = current_line.find(' ');
            int read_write = current_line.substr(0, index) == "r" ? 0 : 1;
            string address = current_line.substr(index);
            int addr = conv_addr(address);
            if(read_write) {
                // write
            } else {
                // read
            }
        }
    }
    return 0;
}

int conv_addr(string addr) {
    int ret_addr = 0;
    int length = addr.length();
    for(int i = 0; i < 8 - length; i++) {
        ret_addr &= 0x0;
        ret_addr <<= 4;
    }
    for(int i = 0; i < length; i++) {
        if(tolower(addr[i]) >= 'a' && tolower(addr[i]) <= 'f') {
            ret_addr |= hexMap[tolower(addr[i]) - 'a' + 10];
            ret_addr <<= 4;
        } else {
            ret_addr |= hexMap[addr[i] - '0'];
            ret_addr <<= 4;
        }
    }
    return ret_addr;
}
