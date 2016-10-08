//
//  main.cpp
//  project1
//
//  Created by Ni Zhang on 10/6/16.
//  Copyright © 2016 Ni Zhang. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdlib>

class Process {
public:
    std::string proc_id;
    int ini_arr_t;//initial arrive time
    int burst_t;//burst_time
    int num_bursts;//number of bursts
    int io_t;//IO running time
    //above gets from the file
    
    int curr_arr_t{curr_arr_t = ini_arr_t}; //current_arrive_time
    int start;
    int end;
    int num_Left;
    int turnaround;
};

bool FCFS_sort(Process proc1, Process proc2){
    return proc1.curr_arr_t <= proc2.curr_arr_t;
}

void FCFS(std::vector<Process> &procs) {
    int t = 0;                  // simulated time
    int n = procs.size(); // number of process to simulate
    
    int nextInput = 0;      // index of first job not in readyQueue
    
    std::vector<Process> READY;      // queue of ready jobs
    std::vector<Process> RUNNING;    // queue of running jobs
    std::vector<Process> BLOCKED;    //queue of io jobs
    
    std::cout << "time " << t << "ms: " << "Start of simulation" << " [Q " << std::endl;//add Queue contents here

    // schedule jobs until no more input and ready queue is empty
    while ((nextInput < procs.size()) || (READY.size() > 0)) {
        
        // add jobs with arrival times <= current time to ready queue
        sort(procs.begin(),procs.end(),FCFS_sort);
        for(int i = 0; i< procs.size(); i++){
            if(procs[i].curr_arr_t <= t && procs[i].num_Left > 0 ){
                READY.push_back(procs[i]);
            }
        }
        
        // if there's anything ready, schedule it
        if (READY.size() > 0) {
            READY.erase(READY.begin());
            std::cout << "time " << t << "ms: " <<READY[0].proc_id.c_str() << "Process starts using the CPU" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<std::endl;
            READY[0].start = t;
            
            RUNNING.push_back(READY[0]);
            
            t += READY[0].burst_t;
            READY[0].end = t;
            
            RUNNING.erase(RUNNING.begin());

            std::cout << "time " << t << "ms: " <<READY[0].proc_id.c_str() << "Process finishes using the CPU" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<std::endl;
            READY[0].turnaround = READY[0].end - READY[0].ini_arr_t;
            
            std::cout << "time " << t << "ms: " <<READY[0].proc_id.c_str() << "Process starts performing I/O" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<std::endl;
            BLOCKED.push_back(READY[0]);
            READY[0].curr_arr_t += READY[0].io_t;// this process goes to io
            std::cout << "time " << t+ READY[0].io_t << "ms: " <<READY[0].proc_id.c_str() << "Process finishes performing" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<std::endl;
            
            
        }
        // otherwise increment time and try again
        /*
        else {
            t = procs[nextInput].ini_arr_t;
        }
         */
    }
}

void read_lines(std::vector<std::string> & proc, std::ifstream & in_str) {
    std::string line;
    while (getline(in_str, line)) {
        if(line.find("#")==0)continue;
        proc.push_back(line);
    }
}

std::vector<std::string> split(std::string str){
    std::string::size_type pos;
    std::vector<std::string> result;
    std::string pattern = "|";
    str+=pattern;
    int size=str.size();
    for(int i=0; i<size; i++){
        pos=str.find(pattern,i);
        if(pos<size){
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

int main(int argc, const char * argv[]) {
    /*
    if (argc != 2) {
        std::cerr <<"Usage: " << argv[0] << "<input file> <stats-output-file>\n";
        return 1;
    }
    */
    std::string input_file(argv[1]);
    std::ifstream in_str(input_file);
    if (!in_str.good()) {
        std::cerr << "Can not open " <<argv[1] << " to read.\n";
        return 1;
    }
    //read the processes from file as lines
    std::vector<std::string> procs;
    read_lines(procs, in_str);
    in_str.close();

    //split the words with "|"
    std::vector<Process> p_container;
    for (int i = 0; i < procs.size(); i++){
        Process p;
        std::vector<std::string> a = split(procs[i]);
        p.proc_id = a[0];
        p.ini_arr_t = std::atoi(a[1].c_str());
        p.burst_t = std::atoi(a[2].c_str());
        p.num_bursts = std::atoi(a[3].c_str());
        p_container.push_back(p);
    }
    FCFS(p_container);
    
    
    //check the output file
    /*
    std::string output_file(argv[2]);
    std::ofstream out_str(output_file);
    if (!out_str.good()){
        std::cerr <<"Can't open " <<argv[2]<<" to write.\n";
        return 1;
    }
    
    */
    return 0;

}
