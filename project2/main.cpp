#include <iostream>
#include <fstream>
#include "OS.h"

int main(int argc, char* argv[]) {
    std::ifstream infile(argv[1]);
    OS myos(infile, 256);
    myos.print();

    return 0;
}
