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

        curr_arr_t = ini_arr_t;
        num_left = num_bursts;
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

//Update ready queue content with current time
void OS::FCFS_update_READY(int t) {
    std::sort(procs.begin(), procs.end(), FCFS_sort);
    for (int i = 0; i < procs.size(); ++i) {
        if (procs[i].curr_arr_t <= t && procs[i].num_left > 0 \
            && std::find(READY.begin(), READY.end(), &procs[i]) == READY.end()) {
            READY.push_back(&procs[i]);
        }
    }
}

void OS::print_READY(int t, const std::string& message) {
    std::cout << "time " << t << "ms: " << message << " [Q ";
    for (std::list<Process*>::const_iterator itr = READY.begin(); itr != READY.end(); ++itr) {
        std::cout << (*itr)->proc_id;
    }
    std::cout<< "]" <<std::endl;
}

void OS::FCFS() {
	int t = 0; // CPU time
    std::cout << "time " << t << "ms: " << "Start of simulation" << std::endl;//add Queue contents here
    // schedule jobs until no more left jobs and ready queue is empty 
    do {
        // add jobs with arrival times <= current time to ready queue
        FCFS_update_READY(t);
        if (!READY.empty()) {
            Process* current = *READY.begin();
            READY.erase(READY.begin());
            RUNNING = current;
            print_READY(t, current->proc_id + " Process starts using the CPU");

            // before increase the time, check if any I/O job is finished
            std::list<Process*>::iterator itr = BLOCKED.begin();
            while (itr != BLOCKED.end()) {
                if (t + current->burst_t >= (*itr)->curr_arr_t) {
                    print_READY(t+current->io_t, current->proc_id + " Process finishes performing I/O");
                    itr = BLOCKED.erase(itr);
                } else {
                    itr++;
                }
            }

            t += current->burst_t;
            RUNNING = NULL; // finish running
            (current->num_left)--;
            current->curr_arr_t = t;
            FCFS_update_READY(t); // After a running job finishes, the READY queue should be updated
            print_READY(t, current->proc_id + " Process finishes using the CPU");
            if (current->num_left == 0) {
                std::cout << current->proc_id << " Process terminates(by finishing its last CPU burst)" << std::endl;
            }

            // Process enters BLOCKED if it needs to
            if (current->io_t) {
                print_READY(t, current->proc_id + " Process starts performing I/O");
                BLOCKED.push_back(current);
                current->curr_arr_t += current->io_t;
            }
        } else if (!BLOCKED.empty()) {
            std::list<Process*>::iterator itr = BLOCKED.begin();
            while (itr != BLOCKED.end()) {
                print_READY((*itr)->curr_arr_t, (*itr)->proc_id + " Process finishes performing I/O");
                itr = BLOCKED.erase(itr);
            }
        }
    } while (!READY.empty() || !BLOCKED.empty());
    std::cout << "End of simulation" <<std::endl;
}




