#include <string>
#include <vector>
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
    int start;
    int end;
    int num_Left;
    int turnaround;

    bool read(std::ifstream & in_str);
	void print();

	//friend bool operator< (const Process& p, const Process& q);
private:
	std::vector<std::string> split(std::string str); // helper function used by read
};

class OS {
private:
    std::vector<Process> READY;      // queue of ready jobs
    std::vector<Process> RUNNING;    // queue of running jobs
    std::vector<Process> BLOCKED;    //queue of io jobs
    std::vector<Process> procs;
public:
	OS(const std::vector<Process>& processes) : procs(processes) {}; // constructor
	// FCFS
	static bool FCFS_sort(const Process& p, const Process& q);
	static int sum_numleft(const std::vector<Process>& processes);
	void FCFS_update_READY(int t);
	bool not_in_READY(const Process& proc);
	void FCFS();
};