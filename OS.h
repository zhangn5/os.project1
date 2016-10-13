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
    std::vector <int> turnaround;
    std::vector <int> waittime;
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
    int num_cs;
    int num_pe;
    int m;
    int ts;
    int sum_turn;
    int sum_wait;
    int sum_burst_t;
    void report_result();
    void reset();
public:
    static bool FCFS_sort(Process* p, Process* q);
    static bool SJF_sort(Process* p, Process* q);
    OS(const std::vector<Process>& processes, const int nprocs = 1, const int time_switch = 8) : \
    RUNNING(NULL), procs(processes), m(nprocs), ts(time_switch) {}; // constructor
    void print_READY(int t, const std::string& message);
    // FCFS
    void FCFS_SJF_update_READY(int t, bool (*sort_procs)(Process* p, Process* q));
    void FCFS_SJF(bool (*sort_procs)(Process* p, Process* q));
    bool FCFS_SJF_all_done();
};
