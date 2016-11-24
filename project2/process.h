#include <vector>

class Process {
public:
    char id;
    int mem;
    std::vector<int> arr_time;
    std::vector<int> run_time;
    int start;
    int finish;
    int burst_left;

    Process(char c, int m, const std::vector<int>& arr, const std::vector<int>& run) : \
        id(c), mem(m), arr_time(arr), run_time(run) {};
};
