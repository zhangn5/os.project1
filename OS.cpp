#include "OS.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <ios>

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
	burst_left = burst_t;
       	return true;
	}
	return false;
}

void Process::print() {
	std::cout << proc_id << '|' << ini_arr_t << '|' << burst_t << '|' << num_bursts << '|' << io_t << std::endl;
}

OS::OS(const std::vector<Process>& processes, const int nprocs , const int time_switch)
{
	procs = processes;
	m = nprocs;
	ts = time_switch;
	time_slice = 0;
	for (unsigned int i = 0; i< procs.size(); ++i) {
	    time_slice = procs[i].burst_t > time_slice ? procs[i].burst_t : time_slice;
	}
}; // constructor for FCFS and SJF

bool OS::FCFS_sort(Process* p, Process* q) {
	return (p->curr_arr_t < q->curr_arr_t || (p->curr_arr_t == q->curr_arr_t && p->proc_id < q->proc_id));
}

bool OS::SJF_sort(Process* p, Process* q) {
	return (p-> burst_t < q-> burst_t || (p->burst_t == q->burst_t && p->proc_id < q->proc_id));
}

//Update ready queue content with current time
void OS::FCFS_SJF_update_READY(int t, bool (*sort_procs)(Process*, Process*)) {
    std::vector<Process*> ready_processes;//newly inserted process
    for (unsigned int i = 0; i < procs.size(); ++i) {
        if (procs[i].curr_arr_t <= t && procs[i].num_left > 0 \
            && std::find(READY.begin(), READY.end(), &procs[i]) == READY.end() && \
		std::find(RUNNING.begin(), RUNNING.end(), &procs[i])== RUNNING.end()) {
            ready_processes.push_back(&procs[i]);
        }
    }

    std::sort(ready_processes.begin(), ready_processes.end(), FCFS_sort);
    for (unsigned int i = 0; i<ready_processes.size(); ++i) { 
	READY.push_back(ready_processes[i]);
    	//sort READY Queue according to the selected algorithm
	READY.sort(sort_procs);
    	//post information if any process is added into READY queue
    	if (ready_processes[i]->num_left == ready_processes[i]->num_bursts && \
	        ready_processes[i]->burst_t == ready_processes[i]->burst_left)
	print_READY(ready_processes[i]->curr_arr_t, "Process " + ready_processes[i]->proc_id + " arrived");
    }
}

void OS::print_READY(int t, const std::string& message) {
    std::cout << "time " << t << "ms: " << message << " [Q";
    if (!READY.empty()) {
    	for (std::list<Process*>::const_iterator itr = READY.begin(); itr != READY.end(); ++itr) {
            std::cout << " " << (*itr)->proc_id;
    	}
    } else {
	std::cout << " empty";
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

void OS::check_io(int t_, Process* current_, bool (*sort_procs_)(Process* p, Process* q)) {
    std::list<Process*>::iterator itr = BLOCKED.begin();
    while (itr != BLOCKED.end()) {
        if (t_ >= (*itr)->curr_arr_t) {
	    FCFS_SJF_update_READY((*itr)->curr_arr_t, sort_procs_);
            print_READY((*itr)->curr_arr_t, "Process " + (*itr)->proc_id + " completed I/O");
            itr = BLOCKED.erase(itr);
        } else {
              itr++;
        }
    }
}

void OS::FCFS_SJF(bool (*sort_procs_)(Process*, Process*)) {
    int t = 0; // CPU time
    RUNNING.clear();
    num_cs = 0;
    num_pe = 0;
    FCFS_SJF_update_READY(t, sort_procs_);
    // schedule jobs until no more left jobs and ready queue is empty 
    do {
        // add jobs with arrival times <= current time to ready queue
        FCFS_SJF_update_READY(t,sort_procs_);
        if (!READY.empty()) {
            Process* current = *READY.begin();
            READY.erase(READY.begin());
	    if (current->burst_left == current -> burst_t) {
	        current->turn_start = current->curr_arr_t;
	    }
	    //check_io(t + ts/2, current, sort_procs_);
	    current->waittime.push_back(t - current->curr_arr_t);
            t += ts/2; // switch in
	    ++num_cs;
            RUNNING.push_back(current);
            print_READY(t, "Process " + current->proc_id + " started using the CPU");


	    bool preempt = current->burst_left > time_slice;
	    while (preempt){
                // before increase the time, check if any I/O job is finished
		check_io(t + time_slice, current, sort_procs_);
		t += time_slice;
		current->burst_left -= time_slice;
		FCFS_SJF_update_READY(t, sort_procs_);
		if (!READY.empty()){
		    READY.push_back(current);
		    ++num_pe;
            	    print_READY(t,"Time slice expired; process " + current->proc_id + \
			" preempted with " + std::to_string(current->burst_left) + "ms to go" );
		    break;
		} else {
		    print_READY(t, "Time slice expired; no preemption because ready queue is empty");
		    preempt = current->burst_left > time_slice;
		} 
	    }

	    if (!preempt) {
		check_io(t + current->burst_left, current, sort_procs_);
            	t += current->burst_left;
		FCFS_SJF_update_READY(t, sort_procs_);
            	(current->num_left)--;
	    	current->turnaround.push_back(t - current->turn_start);
		if (current->num_left > 0) {
            	    print_READY(t, "Process " + current->proc_id + " completed a CPU burst; " + \
				    std::to_string(current->num_left) + " to go");
		    current->burst_left = current->burst_t;
		} else {
                    print_READY(t, "Process " + current->proc_id + " terminated");
		}
	    }

            current->curr_arr_t = t + ts/2;
            FCFS_SJF_update_READY(t, sort_procs_); // After a running job finishes, the READY queue should be updated
	    //check_io(t + ts/2, current, sort_procs_);
            t += ts/2; // switch out
	    FCFS_SJF_update_READY(t, sort_procs_);
            RUNNING.erase(std::find(RUNNING.begin(),RUNNING.end(), current)); // finish running

            // Process enters BLOCKED if it needs to
            if (current->num_left > 0 && current->io_t > 0 && !preempt) {
                print_READY(t - ts/2, "Process " + current->proc_id + " blocked on I/O until time " + std::to_string \
				(t - ts/2 + current->io_t) + "ms");
                BLOCKED.push_back(current);
                current->curr_arr_t = (t + current->io_t - ts/2);
            }
        } else if (!BLOCKED.empty()) {
            BLOCKED.sort(FCFS_sort);
            Process* tmp = *BLOCKED.begin();
            BLOCKED.erase(BLOCKED.begin());
            FCFS_SJF_update_READY(tmp->curr_arr_t, FCFS_sort);
            print_READY(tmp->curr_arr_t, "Process " + tmp->proc_id + " completed I/O");
            t = tmp->curr_arr_t;
        }
    } while (!READY.empty() || !BLOCKED.empty());
    std::cout << "time " << t << "ms: Simulator ended for ";
}

void OS::report_result(const char* filename, const char* algo) {
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
    std::ofstream out_str;
    out_str.open(filename, std::ofstream::out | std::ofstream::app);
    out_str.precision(2);
    out_str << "Algorithm " << algo << std::endl;
    out_str << std::fixed;
    out_str << "-- average CPU burst time: "<< sum_burst_t * 1.0 / sum_burst << std::endl;
    //out_str << std::fixed;
    out_str << "-- average wait time: " << sum_wait * 1.0 / sum_burst << std::endl;
    //out_str << std::fixed;
    out_str << "-- average turnaround time: " <<sum_turn * 1.0 / sum_burst << std::endl;
    out_str << "-- total number of context switches " << num_cs << std::endl;
    out_str << "-- total number of preemption " << num_pe << std::endl;
    out_str.close();
}

void OS::reset() {
    for (unsigned int i = 0; i < procs.size(); ++i) {
	procs[i].curr_arr_t = procs[i].ini_arr_t;
	procs[i].num_left = procs[i].num_bursts;
	procs[i].turnaround.clear();
	procs[i].waittime.clear();
	procs[i].burst_left = procs[i].burst_t;
    }
}
