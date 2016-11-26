#include <iostream>
#include <fstream>
#include "OS.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "ERROR: Invalid arguments" << std::endl;
        std::cout << "USAGE: ./a.out <input-file>" << std::endl;
        return 1;
    }
    std::ifstream infile(argv[1]);
    OS myos(infile, 256);
    myos.print();

    myos.schedule(0);

    return 0;
}
