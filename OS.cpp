#include "OS.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

std::vector<std::string> Process::split(std::string str) {
    std::vector<std::string> result;
    std::string pattern = "|";
    str+=pattern;
    int size=str.size();
    for(int i=0; i<size; i++){
        int pos=str.find(pattern,i);
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

bool OS::SJF_sort(Process* p, Process* q) {
	return (p-> burst_t < q-> burst_t || (p->burst_t == q->burst_t && p->proc_id < q->proc_id));
}

//Update ready queue content with current time
void OS::FCFS_SJF_update_READY(int t, bool (*sort_procs)(Process*, Process*)) {
    std::vector<Process*> ready_processes;
    for (unsigned int i = 0; i < procs.size(); ++i) {
        if (procs[i].curr_arr_t <= t && procs[i].num_left > 0 \
            && std::find(READY.begin(), READY.end(), &procs[i]) == READY.end() && &procs[i] != RUNNING) {
            ready_processes.push_back(&procs[i]);
        }
    }
    std::sort(ready_processes.begin(), ready_processes.end(), sort_procs);
    READY.insert(READY.end(), ready_processes.begin(), ready_processes.end());
    READY.sort(sort_procs);
}

void OS::print_READY(int t, const std::string& message) {
    std::cout << "time " << t << " ms: " << message << " [Q ";
    for (std::list<Process*>::const_iterator itr = READY.begin(); itr != READY.end(); ++itr) {
        std::cout << (*itr)->proc_id;
    }
    std::cout<< "]" <<std::endl;
}

bool OS::FCFS_SJF_all_done() {
    for (unsigned int i = 0; i < procs.size(); ++i) {
        if (procs[i].num_left != 0) {
            return false;
        }
    }
    return true;
}

void OS::FCFS_SJF(bool (*sort_procs_)(Process*, Process*)) {
    int t = 0; // CPU time
    num_cs = 0;
    num_pe = 0;
    FCFS_SJF_update_READY(t, sort_procs_);
    print_READY(t, "Start of simulation");
    // schedule jobs until no more left jobs and ready queue is empty 
    do {
        // add jobs with arrival times <= current time to ready queue
        FCFS_SJF_update_READY(t,sort_procs_);
        if (!READY.empty()) {
            Process* current = *READY.begin();
	    int start = current->curr_arr_t;
            READY.erase(READY.begin());
	    int end_wait = t;
	    current->waittime.push_back(end_wait - start);
            t += ts/2; // switch in
	    ++num_cs;
            RUNNING = current;
            print_READY(t, current->proc_id + " Process starts using the CPU");

            // before increase the time, check if any I/O job is finished
            std::list<Process*>::iterator itr = BLOCKED.begin();
            while (itr != BLOCKED.end()) {
                if (t + current->burst_t >= (*itr)->curr_arr_t) {
                    FCFS_SJF_update_READY((*itr)->curr_arr_t, sort_procs_);
                    print_READY((*itr)->curr_arr_t, (*itr)->proc_id + " Process finishes performing I/O");
                    itr = BLOCKED.erase(itr);
                } else {
                    itr++;
                }
            }
            t += current->burst_t;
            (current->num_left)--;
	    int end_turn = t;
	    current->turnaround.push_back(end_turn - start);
            current->curr_arr_t = t + ts/2;

            FCFS_SJF_update_READY(t, sort_procs_); // After a running job finishes, the READY queue should be updated
            print_READY(t, current->proc_id + " Process finishes using the CPU");
            if (!FCFS_SJF_all_done())
                t += ts/2; // switch out
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
            BLOCKED.sort(FCFS_sort);
            Process* tmp = *BLOCKED.begin();
            BLOCKED.erase(BLOCKED.begin());
            FCFS_SJF_update_READY(tmp->curr_arr_t, FCFS_sort);
            print_READY(tmp->curr_arr_t, tmp->proc_id + " Process finishes performing I/O");
            t = tmp->curr_arr_t;
        }
    } while (!READY.empty() || !BLOCKED.empty());
    print_READY(t, "End of simulation");
    report_result();
    reset(); 
}

void OS::report_result() {
    sum_burst_t = 0;
    sum_turn = 0;
    sum_wait = 0;
    int sum_burst = 0; 
    for (unsigned int i = 0; i < procs.size(); ++i) {
	sum_burst += procs[i].num_bursts;
	for (unsigned int j = 0; j < procs[i].turnaround.size(); ++j) {
	    sum_turn += procs[i].turnaround[j];
	}
	for (unsigned int j = 0; j < procs[i].waittime.size(); ++j) {
	    sum_wait += procs[i].waittime[j];
	}
	sum_burst_t += procs[i].num_bursts * procs[i].burst_t;
    }
    std::cout << "average CPU burst time: " << sum_burst_t * 1.0 / sum_burst << std::endl;
    std::cout << "average wait time: " << sum_wait * 1.0 / sum_burst << std::endl;
    std::cout << "average turnaround time: " << sum_turn * 1.0 / sum_burst << std::endl;
    std::cout << "total number of context switches " << num_cs << std::endl;
    std::cout << "total number of preemption " << num_pe << std::endl;
}

void OS::reset() {
    for (unsigned int i = 0; i < procs.size(); ++i) {
	procs[i].curr_arr_t = procs[i].ini_arr_t;
	procs[i].num_left = procs[i].num_bursts;
	procs[i].turnaround.clear();
	procs[i].waittime.clear();
    }
}
