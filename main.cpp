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
#include <algorithm>

class Process {
public:
    std::string proc_id;
    int ini_arr_t;//initial arrive time
    int burst_t;//burst_time
    int num_bursts;//number of bursts
    int io_t;//IO running time
    //above gets from the file
    
    int curr_arr_t; //current_arrive_time
    int start;
    int end;
    int num_Left;
    int turnaround;
};

//check if a process ID is already in the process vector(queue)
bool not_in(Process proc, std::vector<Process> procs){
    bool not_in = true;
    for (int i = 0; i < procs.size(); i++){
        if(procs[i].proc_id == proc.proc_id){
            not_in = false;
        }
    }
    return not_in;
}

//sum up the left burst times of every job in process vector
int sum_numleft(std::vector<Process> procs){
    int sum=0;
    for (int i = 0; i < procs.size(); i++){
        sum += procs[i].num_Left;
    }
    return sum;
}

//sort the processes with arrival time and if the arrival timeis the same, sort with ID in alphabetical order
bool FCFS_sort(Process proc1, Process proc2){
    return proc1.curr_arr_t < proc2.curr_arr_t || (proc1.curr_arr_t == proc2.curr_arr_t && proc1.proc_id[0] < proc2.proc_id[0]) ;
}

//Update ready queue content with current time
void update_READY(std::vector<Process> &procs, int t, std::vector<Process> &READY){
    std::sort(procs.begin(),procs.end(),FCFS_sort);
    for(std::vector<Process>::iterator itr = procs.begin(); itr != procs.end(); itr++){
        if(itr->curr_arr_t <= t && itr->num_Left > 0 && not_in(*itr,READY)){
            READY.push_back(*itr);
            itr = --(procs.erase(itr));
        }
    }
}

//FCFS(first come first serve)
void FCFS(std::vector<Process> &procs, int t_cs) {
    int t = 0;                  // simulated time
    int num_cs = 0;                 //number of context switches
    
    std::vector<Process> READY;      // queue of ready jobs
    std::vector<Process> RUNNING;    // queue of running jobs
    std::vector<Process> BLOCKED;    //queue of io jobs
    
    //initialize the member variables
    for(int i = 0; i< procs.size(); i++){
        procs[i].curr_arr_t  = procs[i].ini_arr_t;
        procs[i].num_Left = procs[i].num_bursts;
    }
    
    std::cout << "time " << t << "ms: " << "Start of simulation" << std::endl;//add Queue contents here

    // schedule jobs until no more left jobs and ready queue is empty
    bool isStart = true;
    
    while (isStart == true || (READY.size() > 0)|| sum_numleft(procs) > 0){
        isStart = false;
        // add jobs with arrival times <= current time to ready queue
        update_READY(procs,t,READY);

        // if there's anything ready, schedule it
        if (READY.size() > 0) {
            //enter running queue
            RUNNING.push_back(READY[0]);
            READY.erase(READY.begin());
            
            update_READY(procs,t,READY);
            std::cout << "time " << t << "ms: " <<RUNNING[0].proc_id.c_str() << " Process starts using the CPU" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<"]"<<std::endl;
            RUNNING[0].start = t;
            
            t += RUNNING[0].burst_t;
            RUNNING[0].end = t;
            
            update_READY(procs,t,READY);
            std::cout << "time " << t << "ms: " <<RUNNING[0].proc_id.c_str() << " Process finishes using the CPU" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<"]"<<std::endl;
            
            RUNNING[0].turnaround = RUNNING[0].end - RUNNING[0].ini_arr_t;
            
            //this process ran a burst
            RUNNING[0].num_Left --;
            
            if(sum_numleft(READY) == 0){
                std::cout << RUNNING[0].proc_id <<" Process terminates(by finishing its last CPU burst)"<<std::endl<<"End of simulation"<<std::endl;
                break;
                
            }
            
            //enter blocked io queue
            BLOCKED.push_back(RUNNING[0]);
            RUNNING.erase(RUNNING.begin());
            
            update_READY(procs,t,READY);
            std::cout << "time " << t << "ms: " <<BLOCKED[0].proc_id.c_str() << " Process starts performing I/O" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<"]"<<std::endl;

            BLOCKED[0].curr_arr_t = t + BLOCKED[0].io_t; // this process goes to io
            
            procs.push_back(BLOCKED[0]);
            
            update_READY(procs,BLOCKED[0].curr_arr_t,READY);
            std::cout << "time " << BLOCKED[0].curr_arr_t << "ms: " <<BLOCKED[0].proc_id.c_str() << " Process finishes performing I/O" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<"]"<<std::endl;
            BLOCKED.erase(BLOCKED.begin());
            
        }
    }
}

//skip the lines with #, push back the other lines into the vector of string
void read_lines(std::vector<std::string> & proc, std::ifstream & in_str) {
    std::string line;
    while (getline(in_str, line)) {
        if(line.find("#")==0)continue;
        proc.push_back(line);
    }
}

// function to store splitted information of the processes
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
    
    //open the output.txt file to write
    std::string output_file(argv[2]);
    std::ofstream out_str(output_file);
    if (!out_str.good()){
        std::cerr <<"Can't open " <<argv[2]<<" to write.\n";
        return 1;
    }

    //split the words with "|"
    std::vector<Process> p_container;
    for (int i = 0; i < procs.size(); i++){
        Process p;
        std::vector<std::string> a = split(procs[i]);
        p.proc_id = a[0];
        p.ini_arr_t = std::atoi(a[1].c_str());
        p.burst_t = std::atoi(a[2].c_str());
        p.num_bursts = std::atoi(a[3].c_str());
        p.io_t = std::atoi(a[4].c_str());
        p_container.push_back(p);
    }
    
    int t_cs = 8;
    int n = p_container.size(); // number of process to simulate
    
    FCFS(p_container,t_cs);
    
    
    
    
    //check the output file
    

    
    
    return 0;

}
