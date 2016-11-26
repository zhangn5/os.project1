#include <vector>
#include <utility>

class Process {
public:
    char id;
    int mem_size;
    std::vector<int> arr_time;
    std::vector<int> run_time;
    int start;
    int finish;
    int burst_left;
    std::vector<std::pair<int, int> > positions;

    Process(char c, int m, const std::vector<int>& arr, const std::vector<int>& run) : \
        id(c), mem_size(m), arr_time(arr), run_time(run), \
        start(arr[0]), finish(arr[0]+run[0]), burst_left(arr.size()) {};
};
