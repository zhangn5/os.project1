#include <cstdlib>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "OS.h"

// Helper function to remove the leading and trailing whitespace from a string
void OS::trim(std::string& s) 
{
    size_t p = s.find_first_not_of(" \t\r");
    s.erase(0, p);
    p = s.find_last_not_of(" \t\r");
    if (std::string::npos != p)
        s.erase(p+1);
}

bool OS::is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void OS::print() {
    for (auto& p : procs) {
        std::cout << p.id << " " << p.mem_size << " ";
        for (int i = 0; i < p.arr_time.size(); ++i) {
            std::cout << p.arr_time[i] << "/" << p.run_time[i] << " ";
        }
        std::cout << std::endl;
    }
}

bool OS::sort_by_start(Process* a, Process*b) {
    return (a->start < b->start || (a->start == b->start && a->id < b->id));
}

bool OS::sort_by_finish(Process* a, Process*b) {
    return (a->finish < b->finish || (a->finish == b->finish && a->id < b->id));
}

bool OS::sort_pair(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return (a.first < b.first);
}

bool OS::sort_by_position(Process* a, Process* b) {
    return a->positions[0].first < b->positions[0].first;
}

OS::OS(std::ifstream& infile, int memory_size) {
    std::string line;
    int n;
    while (std::getline(infile, line)) {
        trim(line);
        if (line[0] == '#') continue;
        if (is_number(line)) 
            n = atoi(line.c_str());
        else {
            std::stringstream ss;
            ss.str(line);
            std::string temp;
            char c;
            int m;
            std::vector<int> arr, run;
            int i = 0;
            while (ss >> temp) {
                if (i == 0) {
                    c = temp[0];
                } else if (i == 1) {
                    m = atoi(temp.c_str());
                } else {
                    size_t pos = temp.find('/');
                    arr.push_back(atoi(temp.substr(0, pos).c_str()));
                    run.push_back(atoi(temp.substr(pos+1).c_str()));
                }
                i++;
            }
            procs.push_back(Process(c, m, arr, run));
            waiting.push_back(&procs[procs.size()-1]);
        }
    }
    memory = std::string(".", memory_size);
    clock = 0;
    most_recent = -1;
    free_memory.push_back(std::make_pair(0, memory_size-1));

    waiting.sort(sort_by_start);
    free_memory.sort(sort_pair);
}

void OS::print_memory() {
    std::string temp('=', 32);
    std::cout << temp << std::endl;
    for (int i = 0; i < 8; ++i) {
        std::cout << memory.substr(memory.size()/8*i, memory.size()/8) << std::endl;
    }
    std::cout << temp << std::endl;
}

void OS::finish_process() {
    clock = (*running.begin())->finish;
    auto p = *running.begin();
    for (auto pos : p->positions) {
        for (int i = pos.first; i <= pos.second; ++i) {
            memory[i] = '.';
        }
        auto q = free_memory.begin();
        while (q != free_memory.end()) {
            if (q->first > pos.first) {
                q = free_memory.insert(q, pos);
                break;
            }
            q++;
        }
        if (q == free_memory.end()) {
            q = free_memory.insert(q, pos);
        }
        // merge if necessary
        if (q != free_memory.begin()) {
            auto prev = q;
            prev--;
            if (q->first == prev->second) {
                q->first = prev->first;
                free_memory.erase(prev);
            }
        }
        auto post = q;
        post++;
        if (post != free_memory.end())
        {
            if (q->second == post->first) {
                q->second = post->second;
                free_memory.erase(post);
            }
        }
    }
    if (--(p->burst_left) != 0) {
        p->start = p->arr_time[p->arr_time.size() - p->burst_left];
        p->finish = p->start + p->run_time[p->arr_time.size() - p->burst_left];
        auto itr = waiting.begin();
        while (itr != waiting.end()) {
            if ((*itr)->start > p->start) {
                itr = waiting.insert(itr, p);
                break;
            }
            itr++;
        }
        if (itr == waiting.end()) {
            itr = waiting.insert(itr, p);
        }
    }
    std::cout << "time " << clock << "ms: Process " << p->id << " removed:" << std::endl;
    print_memory();
    p->positions.pop_back();
    running.erase(running.begin());
}

void OS::start_process(int type) {
    clock = (*waiting.begin())->start;
    auto p = *waiting.begin();
    switch(type) {
        case 0: { // next-fit
            bool placed = false;
            int free_memory_size = 0;
            for (auto q = free_memory.begin(); q != free_memory.end(); ++q) {
                free_memory_size += (q->second - q->first + 1);
                if (q->first > most_recent && q->second - most_recent >= p->mem_size) {
                    for (int i = q->first; i < q->first + p->mem_size; ++i) {
                        memory[i] = p->id;
                    }
                    p->positions.push_back(std::pair<int, int>(std::make_pair(q->first, q->first+p->mem_size-1)));
                    most_recent = q->first + p->mem_size - 1;
                    q->first = most_recent + 1;
                    if (q->first == q->second + 1) {
                        free_memory.erase(q);
                    }
                    placed = true;
                    running.push_back(p);
                    running.sort(sort_by_finish);
                    break;
                }
            }
            if (!placed) {
                if (p->mem_size <= free_memory_size) {
                    // defragmentation
                    std::cout << "time " << clock << "ms: " << "Cannot place process " \
                        << p->id << " -- starting defragmentation" << std::endl;
                    defragmentation();
                } else {
                    if (--(p->burst_left) != 0) {
                        p->start = p->arr_time[p->arr_time.size() - p->burst_left];
                        p->finish = p->start + p->run_time[p->arr_time.size() - p->burst_left];
                        // re-sort waiting
                        waiting.sort(sort_by_start);
                    }
                    std::cout << "time " << clock << "ms: Cannot place process " << p->id << " -- skipped!" << std::endl;
                }
            }
            break;
        }
        case 1: { // best-fit
            break;
        }
        case 2: { // worst-fit
            break;
        }
        case 3: { // non-contiguous
            break;
        }
    }
}

void OS::schedule(int type) {
    do {
        // find the next interesting event
        if (!running.empty() && !waiting.empty()) {
            if ((*running.begin())->finish <= (*waiting.begin())->start) {
                finish_process();
            } else {
                start_process(type);
            }
        } else if (running.empty() && !waiting.empty()) {
            start_process(type);
        } else if (!running.empty() && waiting.empty()) {
            finish_process();
        }
    } while (!running.empty() || !waiting.empty());
}

void OS::defragmentation() {
    auto memory_copy = memory;
    auto running_copy = running;
    running_copy.sort(sort_by_position);
    
    std::vector<Process*> shuffled_process;
    int current_position = 0;
    for (auto ptr : running_copy) {
        int size = ptr->positions[0].second - ptr->positions[0].first + 1;
        for (int i = current_position; i <= current_position+size-1; ++i) {
            memory[i] = ptr->id;
        }
        if (ptr->positions[0].first != current_position || \
                ptr->positions[0].second != current_position + size - 1) {
            ptr->positions[0].first = current_position;
            ptr->positions[0].second = current_position + size - 1;
            shuffled_process.push_back(ptr);
        }
        current_position += size;
    }
    for (int i = current_position; i < memory.size(); ++i) {
        memory[i] = '.';
    }

    int count_diff = 0;
    for (int i = 0; i < memory.size(); ++i) {
        if (memory[i] != memory_copy[i]) {
            count_diff++;
        }
    }

    clock += count_diff;
    for (auto& proc : procs) {
        for (auto& t : proc.arr_time) t += count_diff;
        for (auto& t : proc.arr_time) t += count_diff;
        proc.start += count_diff;
        proc.finish += count_diff;
    }

    std::cout << "time " << clock << "ms: Defragmentation complete (moved " \
        << count_diff << " frames: ";
    for (int i = 0; i < shuffled_process.size(); ++i) {
        std::cout << shuffled_process[i]->id;
        if (i != shuffled_process.size()-1) {
            std::cout << ", ";
        } else {
            std::cout << ")" << std::endl;
        }
    }
    print_memory();
}























