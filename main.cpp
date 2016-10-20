#include <iostream>
#include <fstream>
#include "OS.h"

int main(int argc, const char * argv[]) {
    if (argc != 3) {
        std::cerr <<"Usage: " << argv[0] << "<input file> <stats-output-file>\n";
        return 1;
    }
    
    std::ifstream in_str(argv[1]);
    if (!in_str.good()) {
        std::cerr << "Can not open " <<argv[1] << " to read." << std::endl;
        return 1;
    }

    std::ofstream out_str(argv[2]);
    if (!out_str.good()) {
	std::cerr << "Can not open" <<argv[2] << " to write." << std::endl;
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

    OS myos(p_container, 1, 8);

    std::cout << "time 0ms: Simulator started for FCFS [Q empty]" << std::endl;
    myos.FCFS_SJF(OS::FCFS_sort);
    std::cout << "FCFS" << std::endl;
    std::cout << std::endl;
    myos.report_result(argv[2], "FCFS");
    myos.reset();

    std::cout << "time 0ms: Simulator started for SJF [Q empty]" << std::endl;
    myos.FCFS_SJF(OS::SJF_sort);
    std::cout << "SJF" << std::endl;
    std::cout << std::endl;
    myos.report_result(argv[2], "SJF");
    myos.reset();



    OS myos2(p_container,1, 8, 84);
    std::cout << "time 0ms: Simulator started for RR [Q empty]" << std::endl;
    myos2.FCFS_SJF(OS::FCFS_sort);
    std::cout << "RR" << std::endl;
    myos2.report_result(argv[2], "RR");
    myos.reset();
    return 0;
}
