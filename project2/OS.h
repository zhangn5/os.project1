#include <string>
#include <vector>
#include <utility>
#include <list>
#include <fstream>
#include <set>
#include "process.h"

class OS {
private:
    // fuctors used as custom comparators in std::set
    struct sort_by_finish {
        bool operator()(const Process* a, const Process* b) {
            return a->finish < b->finish || (a->finish == b->finish && a->id < b-> id);
        }
    };
    struct sort_by_start {
        bool operator()(const Process* a, const Process* b) {
            return a->start < b->start || (a->start == b->start && a->id < b-> id);
        }
    };
    void trim(std::string& s);
    bool is_number(const std::string& s);
private:
    std::vector<Process> procs;
    std::string memory;
    int clock;
    std::list<std::pair<int, int> > free_memory;
    std::set<Process*, sort_by_finish> running;
    std::set<Process*, sort_by_start> waiting;
public:
    OS(std::ifstream& infile, int memory_size);
    void print();
};
