#include <cstdlib>
#include <iostream>
#include <sstream>
#include "OS.h"

// Helper function to remove the leading and trailing whitespace from a string
void OS::trim(std::string& s) 
{
    size_t p = s.find_first_not_of(" \t\r");
    s.erase(0, p);
    p = s.find_last_not_of(" \t\r");
    if (std::string::npos != p)
        s.erase(p+1);
}

bool OS::is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void OS::print() {
    for (auto& p : procs) {
        std::cout << p.id << " " << p.mem << " ";
        for (int i = 0; i < p.arr_time.size(); ++i) {
            std::cout << p.arr_time[i] << "/" << p.run_time[i] << " ";
        }
        std::cout << std::endl;
    }
}

OS::OS(std::ifstream& infile, int memory_size) {
    std::string line;
    int n;
    while (std::getline(infile, line)) {
        trim(line);
        if (line[0] == '#') continue;
        if (is_number(line)) 
            n = atoi(line.c_str());
        else {
            std::stringstream ss;
            ss.str(line);
            std::string temp;
            char c;
            int m;
            std::vector<int> arr, run;
            int i = 0;
            while (ss >> temp) {
                if (i == 0) {
                    c = temp[0];
                } else if (i == 1) {
                    m = atoi(temp.c_str());
                } else {
                    size_t pos = temp.find('/');
                    arr.push_back(atoi(temp.substr(0, pos).c_str()));
                    run.push_back(atoi(temp.substr(pos+1).c_str()));
                }
                i++;
            }
            procs.push_back(Process(c, m, arr, run));
            waiting.insert(&procs[procs.size()-1]);
        }
    }
    memory = std::string(".", memory_size);
    clock = 0;
    free_memory.push_back(std::make_pair(0, memory_size-1));
}

/*
OS::schedule() {
    // scan procs, compare with clock
    // load to memory, update free_memory
    // call update
    while (not_done()) {
        update();
        for (auto p : procs) {
            if (p.arr_time <= clock)
        }
    }
}

OS::update() {
    // find the next interesting event
    if (!running.empty()) {
        if (running.begin()->finish < waiting.begin()->start) {
        } else {
        }
    }
}
*/









