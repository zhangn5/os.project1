#include "OS.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

std::vector<std::string> Process::split(std::string str) {
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

bool Process::read(std::ifstream& in_str) {
	std::string line;
	while (std::getline(in_str, line)) {
		if (line[0] == '#') continue;
		std::vector<std::string> a = split(line);
        proc_id = a[0];
       	ini_arr_t = std::atoi(a[1].c_str());
       	burst_t = std::atoi(a[2].c_str());
       	num_bursts = std::atoi(a[3].c_str());
       	io_t = std::atoi(a[4].c_str());
       	return true;
	}
	return false;
}

void Process::print() {
	std::cout << proc_id << '|' << ini_arr_t << '|' << burst_t << '|' << num_bursts << '|' << io_t << std::endl;
}

bool OS::FCFS_sort(const Process& p, const Process& q) {
	return (p.curr_arr_t < q.curr_arr_t || (p.curr_arr_t == q.curr_arr_t && p.proc_id < q.proc_id));
}

//check if a process ID is already in the process vector(queue)
bool OS::not_in_READY(const Process& proc){
    bool not_in = true;
    for (int i = 0; i < READY.size(); i++){
        if(READY[i].proc_id == proc.proc_id){
            not_in = false;
        }
    }
    return not_in;
}

//Update ready queue content with current time
void OS::FCFS_update_READY(int t) {
    std::sort(procs.begin(), procs.end(), FCFS_sort);
    for(std::vector<Process>::iterator itr = procs.begin(); itr != procs.end(); itr++){
        if(itr->curr_arr_t <= t && itr->num_Left > 0 && not_in_READY(*itr)){
            READY.push_back(*itr);
            itr = --(procs.erase(itr));
        }
    }
}

//sum up the left burst times of every job in process vector
int OS::sum_numleft(const std::vector<Process>& processes){
    int sum=0;
    for (int i = 0; i < processes.size(); i++){
        sum += processes[i].num_Left;
    }
    return sum;
}

void OS::FCFS() {
	int t = 0;                  // simulated time
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
        FCFS_update_READY(t);

        // if there's anything ready, schedule it
        if (READY.size() > 0) {
            //enter running queue
            RUNNING.push_back(READY[0]);
            READY.erase(READY.begin());
            
            FCFS_update_READY(t);
            std::cout << "time " << t << "ms: " <<RUNNING[0].proc_id.c_str() << " Process starts using the CPU" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<"]"<<std::endl;
            RUNNING[0].start = t;
            
            t += RUNNING[0].burst_t;
            RUNNING[0].end = t;
            
            FCFS_update_READY(t);
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
            
            FCFS_update_READY(t);
            std::cout << "time " << t << "ms: " <<BLOCKED[0].proc_id.c_str() << " Process starts performing I/O" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<"]"<<std::endl;

            BLOCKED[0].curr_arr_t = t + BLOCKED[0].io_t; // this process goes to io
            
            procs.push_back(BLOCKED[0]);
            
            FCFS_update_READY(BLOCKED[0].curr_arr_t);
            std::cout << "time " << BLOCKED[0].curr_arr_t << "ms: " <<BLOCKED[0].proc_id.c_str() << " Process finishes performing I/O" << " [Q ";
            for (int a = 0; a < READY.size(); a++){
                std::cout<< READY[a].proc_id;
            }
            std::cout<<"]"<<std::endl;
            BLOCKED.erase(BLOCKED.begin());
            
        }
    }
}