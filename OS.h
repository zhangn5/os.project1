#include <string>
#include <vector>
#include <list>
#include <fstream>

class Process {
public:
    std::string proc_id;
    int ini_arr_t;//initial arrive time
    int burst_t;//burst_time
    int num_bursts;//number of bursts
    int io_t;//IO running time
    //above gets from the file
    
    int curr_arr_t; //current_arrive_time
    int num_left;
    int turnaround;

    bool read(std::ifstream & in_str);
	void print();
private:
	std::vector<std::string> split(std::string str); // helper function used by read
};

class OS {
private:
    std::list<Process*> READY;      // queue of ready jobs
    Process* RUNNING;               // the running job, currently there is only 1
    std::list<Process*> BLOCKED;    // queue of io jobs
    std::vector<Process> procs;
    int m;
public:
	OS(const std::vector<Process>& processes, const int nprocs = 1) : \
        RUNNING(NULL), procs(processes), m(nprocs) {}; // constructor
    void print_READY(int t, const std::string& message);
	// FCFS
	static bool FCFS_sort(const Process& p, const Process& q);
	void FCFS_update_READY(int t);
	void FCFS();
};