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

bool OS::FCFS_sort(Process* p, Process* q) {
	return (p->curr_arr_t < q->curr_arr_t || (p->curr_arr_t == q->curr_arr_t && p->proc_id < q->proc_id));
}

//Update ready queue content with current time
void OS::FCFS_update_READY(int t) {
    //std::sort(ready_procs.begin(), ready_procs.end(), FCFS_sort);
    std::vector<Process*> ready_processes;
    for (int i = 0; i < procs.size(); ++i) {
        if (procs[i].curr_arr_t <= t && procs[i].num_left > 0 \
            && std::find(READY.begin(), READY.end(), &procs[i]) == READY.end() && &procs[i] != RUNNING) {
            ready_processes.push_back(&procs[i]);
        }
    }
    std::sort(ready_processes.begin(), ready_processes.end(), FCFS_sort);
    READY.insert(READY.end(), ready_processes.begin(), ready_processes.end());
}

void OS::FCFS_clear_BLOCKED(int& t) {
    BLOCKED.sort(FCFS_sort);
    std::list<Process*>::iterator itr = BLOCKED.begin();
    while (itr != BLOCKED.end()) {
	Process *tmp = *itr;
        itr = BLOCKED.erase(itr);
        FCFS_update_READY(tmp->curr_arr_t);
        print_READY(tmp->curr_arr_t, tmp->proc_id + " Process finishes performing I/O");
	t = tmp->curr_arr_t;
	break;
    }
}

void OS::print_READY(int t, const std::string& message) {
    std::cout << "time " << t << " ms: " << message << " [Q ";
    for (std::list<Process*>::const_iterator itr = READY.begin(); itr != READY.end(); ++itr) {
        std::cout << (*itr)->proc_id;
    }
    std::cout<< "]" <<std::endl;
}

void OS::FCFS() {
    // echo the procs
    for (std::vector<Process>::iterator itr = procs.begin(); itr != procs.end(); ++itr) {
        itr->print();
    }

	int t = 0; // CPU time
    std::cout << "time " << t << "ms: " << "Start of simulation" << std::endl;//add Queue contents here
    // schedule jobs until no more left jobs and ready queue is empty 
    do {
        // add jobs with arrival times <= current time to ready queue
        FCFS_update_READY(t);
        if (!READY.empty()) {
            Process* current = *READY.begin();
            READY.erase(READY.begin());
            t += ts/2;
            RUNNING = current;
            print_READY(t, current->proc_id + " Process starts using the CPU");

            // before increase the time, check if any I/O job is finished
            std::list<Process*>::iterator itr = BLOCKED.begin();
            while (itr != BLOCKED.end()) {
                if (t + current->burst_t >= (*itr)->curr_arr_t) {
                    FCFS_update_READY((*itr)->curr_arr_t);
                    print_READY((*itr)->curr_arr_t, (*itr)->proc_id + " Process finishes performing I/O");
                    itr = BLOCKED.erase(itr);
                } else {
                    itr++;
                }
            }

            t += current->burst_t;
            (current->num_left)--;
            current->curr_arr_t = t + ts/2;

            FCFS_update_READY(t); // After a running job finishes, the READY queue should be updated
            print_READY(t, current->proc_id + " Process finishes using the CPU");
            t += ts/2;
            RUNNING = NULL; // finish running

            if (current->num_left == 0) {
                print_READY(t, current->proc_id + " Process terminates (by finishing its last CPU burst)");
            }

            // Process enters BLOCKED if it needs to
            if (current->num_left > 0 && current->io_t > 0) {
                print_READY(t, current->proc_id + " Process starts performing I/O");
                BLOCKED.push_back(current);
                current->curr_arr_t = (t + current->io_t - ts/2);
            }
        } else if (!BLOCKED.empty()) {
            FCFS_clear_BLOCKED(t);
        }
    } while (!READY.empty() || !BLOCKED.empty());
    std::cout << "End of simulation" <<std::endl;
}




