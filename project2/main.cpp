#include <iostream>
#include <fstream>
#include "OS.h"
#include "VM.h"
#include <vector>
#include <map>
#include <algorithm>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "ERROR: Invalid arguments" << std::endl;
        std::cout << "USAGE: ./a.out <input-file> <vm-input-file>" << std::endl;
        return 1;
    }
    std::ifstream infile;
    infile.open(argv[1]);
    OS myos(infile, 256);
    myos.schedule(0);
    std::cout << std::endl;
    
    infile.open(argv[1]);
    OS myos1(infile, 256);
    myos1.schedule(1);
    std::cout << std::endl;

    infile.open(argv[1]);
    OS myos2(infile, 256);
    myos2.schedule(2);
    std::cout << std::endl;

    infile.open(argv[1]);
    OS myos3(infile, 256);
    myos3.schedule(3);
    std::cout << std::endl;

    std::ifstream page_str(argv[2]);
    
    if(!page_str){
        std::cerr << "Can not open the page file " << argv[1] << std::endl;
        return 1;
    }
    VM pages_in(page_str);
    pages_in.OPT();
    std::cout << std::endl;

    pages_in.LRU();
    std::cout << std::endl;

    pages_in.LFU();

    
    return 0;
}
