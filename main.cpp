#include <iostream>
#include <fstream>
#include "OS.h"

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        std::cerr <<"Usage: " << argv[0] << "<input file> <stats-output-file>\n";
        return 1;
    }
    
    std::ifstream in_str(argv[1]);
    if (!in_str.good()) {
        std::cerr << "Can not open " <<argv[1] << " to read.\n";
        return 1;
    }

    //read the processes from file as lines
    std::vector<Process> p_container;
    while (1) {
    	Process p;
    	if (p.read(in_str)) {
    		p_container.push_back(p);
    	} else {
    		break;
    	}
    }
    in_str.close();

    OS myos(p_container);
    myos.FCFS();

    return 0;
}