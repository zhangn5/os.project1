//
//  VM.hpp
//  project2
//
//  Created by Ni Zhang on 12/11/16.
//  Copyright © 2016 Ni Zhang. All rights reserved.
//

#ifndef VM_hpp
#define VM_hpp

#include <vector>
#include <iostream>
#include <fstream>

class VM{
public:
    VM(std::ifstream & page_str);
    void OPT();
    void LRU();
    void LFU();
    
    void print_frame(const std::vector<int> &frame, unsigned int F, const std::vector<int>::iterator &f_itr, int victim, bool not_full, bool found_in_frame,std::vector<bool>& page_fault);
private:

    std::vector<int> pages_in;

};
#endif /* VM_hpp */
