#include <string>
#include <vector>
#include <list>
#include <set>
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
    int burst_left;
    int num_left;
    int turn_start;
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
    std::set<Process*> RUNNING;
    std::list<Process*> BLOCKED;    // queue of io jobs
    std::vector<Process> procs;
    int num_cs;
    int num_pe;
    int nprocs;
    int ts;
    int sum_turn;
    int sum_wait;
    int sum_burst_t;
    int time_slice;
public:
    static bool FCFS_sort(Process* p, Process* q);
    static bool SJF_sort(Process* p, Process* q);
    OS(const std::vector<Process>& processes, const int m, const int time_switch, const int time_slice_ ) : \
    procs(processes), nprocs(m), ts(time_switch), time_slice(time_slice_) {}; // constructor for RR

    OS(const std::vector<Process>& processes, const int m, const int time_switch );// constructor for FCFS and SJF
    void print_READY(int t, const std::string& message);
    // FCFS
    void FCFS_SJF_update_READY(int t, bool (*sort_procs)(Process* p, Process* q));
    void FCFS_SJF(bool (*sort_procs)(Process* p, Process* q));
    bool FCFS_SJF_all_done();
    void check_io(int, Process*, bool (*sort_procs)(Process* p, Process* q));
    void report_result(const char*, const char* );
    void reset();
};

