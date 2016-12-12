//
//  VM.cpp
//  project2
//
//  Created by Ni Zhang on 12/11/16.
//  Copyright © 2016 Ni Zhang. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include "VM.h"

typedef std::vector<std::pair<int, int> > VEC_PAIR;
typedef std::pair<int, int> PAIR;
typedef std::map<int, int> MAP;

bool sort_largest(const PAIR & pair1, const PAIR & pair2){
    return pair1.first < pair2.first || ((pair1.first == pair2.first) && pair1.second > pair2.second);
}

bool sort_largest_2(const PAIR & pair1, const PAIR & pair2){
    return pair1.first < pair2.first || ((pair1.first == pair2.first) && pair1.second < pair2.second);
}

bool in_frame(const std::vector<int> &frame, const std::vector<int>::iterator &f_itr){
    bool found = false;
    for(unsigned int i = 0; i < frame.size(); i++){
        if(*f_itr == frame[i]){
            found = true;
        }
    }
    return found;
}


VM::VM(std::ifstream & page_str){
    int x;
    while(page_str >> x){
        this->pages_in.push_back(x);
    }
}


void VM::print_frame(const std::vector<int> &frame, unsigned int F, const std::vector<int>::iterator &f_itr, int victim, bool not_full, bool found_in_frame,std::vector<bool>& page_fault){
    
    //if the incoming page is not found the page in the frame or
    //the frame has space to store the page and it's a page fault
    std::cout << "referencing page "<< *f_itr << " [mem: ";
    unsigned int i = 0;
    for(; i < frame.size(); i++){
        std::cout << frame[i];
        if(i < frame.size() - 1){
            std::cout << " ";
        }
    }
    while(i < F){
        std::cout << " .";
        i++;
    }
    std::cout <<"]";
    
    std::cout<< " PAGE FAULT";
    
    if(not_full || found_in_frame){
        std::cout << " (no victim page)"<<std::endl;
    }else{
        std::cout << " (victim page " << victim <<")"<<std::endl;
    }
    
}
//The Optimal (OPT) algorithm is a forward-looking algorithm that selects the “victim” page by
//identifying the frame that will be accessed the longest time in the future (or not at all).
void VM::OPT(){
    unsigned int F = 3;
    int largest = 0;
    std::cout << "Simulating OPT with fixed frame size of " << F << std::endl;
    std::vector<int> frame;
    std::vector<int>::iterator f_itr = pages_in.begin();
    std::vector<bool> page_fault;
    int page_fault_counter = 0;
    
    while(frame.size() < F && f_itr != pages_in.end()){
        bool not_full = true;
        bool found_in_frame = in_frame(frame, f_itr);
        if(!found_in_frame){
            frame.push_back(*f_itr);
            print_frame(frame,F,f_itr,largest,not_full,found_in_frame,page_fault);
            page_fault.push_back(true);
            page_fault_counter++;
        }else{
            page_fault.push_back(false);
            
        }
        
        f_itr++;
    }
    
    //for each member in the frame, count the distance from it to the nearest in the future and insert into a map
    while(f_itr!= pages_in.end()){
        bool not_full = false;
        bool found_in_frame = in_frame(frame, f_itr);
        if(!found_in_frame){
            
            VEC_PAIR distances;
            for(unsigned int i = 0; i < frame.size(); i++){
                std::vector<int>::iterator tmp = f_itr;
                int loc = 0;
                bool found_in_rest = false;
                while(tmp!= pages_in.end()){
                    if(frame[i] == *tmp && !found_in_rest){
                        
                        distances.push_back(std::make_pair(loc,frame[i]));
                        found_in_rest = true;
                    }
                    loc++;
                    tmp++;
                }
                //if the pages in frame aren't one of the incoming page, then the loc is the furthest
                if(found_in_rest == false){
                    distances.push_back(std::make_pair(loc,frame[i]));
                }
                
            }
            std::sort(distances.begin(),distances.end(), sort_largest);
            
            //take the largest distance one to be the page to replace
            largest = distances.rbegin()->second;
            for(unsigned int i = 0; i < frame.size(); i++){
                if(frame[i] == largest){
                    frame[i] = *f_itr;
                }
            }
        }
        if(!found_in_frame){
            print_frame(frame,F,f_itr,largest,not_full,found_in_frame,page_fault);
            page_fault.push_back(true);
            page_fault_counter++;
            
        }else{
            page_fault.push_back(false);
            
        }
        f_itr++;
    }
    std::cout << "End of OPT simulation ("<< page_fault_counter <<" page faults)"<< std::endl;
}

//least recently used algorithm
//a backward-looking algorithm that selects the “victim” page by identifying the frame that has the oldest access time.
void VM::LRU(){
    unsigned int F = 3;
    int largest = 0;
    std::cout << "Simulating LRU with fixed frame size of " << F << std::endl;
    std::vector<int> frame;
    std::vector<int>::iterator f_itr = pages_in.begin();
    std::vector<bool> page_fault;
    int page_fault_counter = 0;
    
    //when the frame is not full, we check if the page is in the frame and if not add it and print it
    while(frame.size() < F && f_itr != pages_in.end()){
        bool not_full = true;
        bool found_in_frame = in_frame(frame, f_itr);
        if(!found_in_frame){
            frame.push_back(*f_itr);
            
            print_frame(frame,F,f_itr,largest,not_full,found_in_frame,page_fault);
            page_fault.push_back(true);
            page_fault_counter++;
            
        }else{
            page_fault.push_back(false);
            
        }
        
        
        f_itr++;
    }
    
    //when the frame is full go to the next incoming page and check if the page is in the frame
    while(f_itr!= pages_in.end()){
        bool not_full = false;
        bool found_in_frame = in_frame(frame, f_itr);
        //if not in the frame, we check the pages in frame and count the distance to the nearest one in the future
        if(!found_in_frame){
            
            VEC_PAIR distances;
            for(unsigned int i = 0; i < frame.size(); i++){
                std::vector<int>::iterator tmp = f_itr;
                int loc = 0;
                bool found_in_rest = false;
                while(tmp!= pages_in.begin()){
                    if(frame[i] == *tmp && !found_in_rest){
                        distances.push_back(std::make_pair(loc,frame[i]));
                        found_in_rest = true;
                    }
                    loc++;
                    tmp--;
                }
                
                if(found_in_rest == false){
                    //if the reference page is at the pages_in.begin()
                    if(frame[i] == *tmp){
                        distances.push_back(std::make_pair(loc,frame[i]));
                    }else{//if the pages in frame aren't one of the incoming page, then the loc is the furthest
                        loc++;
                        distances.push_back(std::make_pair(loc,frame[i]));
                    }
                }
                
            }
            std::sort(distances.begin(),distances.end(), sort_largest);
            
            //take the largest distance one to be the page to replace
            largest = distances.rbegin()->second;
            for(unsigned int i = 0; i < frame.size(); i++){
                if(frame[i] == largest){
                    frame[i] = *f_itr;
                }
            }
        }
        if(!found_in_frame){
            print_frame(frame,F,f_itr,largest,not_full,found_in_frame,page_fault);
            page_fault.push_back(true);
            page_fault_counter++;
            
            
        }else{
            page_fault.push_back(false);
            
        }
        f_itr++;
    }
    
    std::cout << "End of LRU simulation ("<< page_fault_counter <<" page faults)"<< std::endl;
    
}

//Least-Frequently Used (LFU) algorithm
//a backward-looking algorithm that selects the “victim” page by identifying the frame with the lowest number of accesses. When a page fault occurs for a given page, its reference count is set (or reset) to 1; each subsequent access increments this reference count.

void VM::LFU(){
    unsigned int F = 3;
    int smallest = 0;
    std::cout << "Simulating LFU with fixed frame size of " << F << std::endl;
    std::vector<int> frame;
    std::vector<int>::iterator f_itr = pages_in.begin();
    std::vector<bool> page_fault;
    int page_fault_counter = 0;
    
    //when the frame is not full, we check if the page is in the frame and if not add it and print it
    while(frame.size() < F && f_itr != pages_in.end()){
        bool not_full = true;
        bool found_in_frame = in_frame(frame, f_itr);
        if(!found_in_frame){
            frame.push_back(*f_itr);
            print_frame(frame,F,f_itr,smallest,not_full,found_in_frame,page_fault);
            page_fault.push_back(true);
            page_fault_counter++;
            
            
        }else{
            page_fault.push_back(false);
            
        }
        f_itr++;
    }
    //when the frame is full go to the next incoming page and check if the page is in the frame
    
    while(f_itr!= pages_in.end()){
        bool not_full = false;
        bool found_in_frame = in_frame(frame, f_itr);
        
        //if not in the frame, we check the pages in frame and count the times it accessed in the past
        if(!found_in_frame){
            
            MAP counters;
            for(unsigned int i = 0; i < frame.size(); i++){
                std::vector<int>::iterator tmp = pages_in.begin();
                std::vector<bool>::iterator pf_itr = page_fault.begin();
                
                bool found_in_rest = false;
                while(tmp!= f_itr){
                    if(frame[i] == *tmp && *pf_itr == false){
                        ++counters[frame[i]];
                        found_in_rest = true;
                        
                    }else if(frame[i] == *tmp && *pf_itr == true){//When a page fault occurs for a given page, its reference count is set (or reset) to 1
                        counters[frame[i]] = 1;
                        found_in_rest = true;
                    }
                    
                    tmp++;
                    pf_itr++;
                }
                
                if(found_in_rest == false){
                    counters[frame[i]] = 0;
                }
                
            }
            VEC_PAIR collection;
            for(MAP::iterator m_iter = counters.begin(); m_iter != counters.end(); m_iter++ ){
                collection.push_back(std::make_pair(m_iter->second,m_iter->first));
            }
            std::sort(collection.begin(), collection.end(), sort_largest_2);
            smallest = collection.begin()->second;
            for(unsigned int i = 0; i < frame.size(); i++){
                if(frame[i] == smallest){
                    frame[i] = *f_itr;
                }
            }
        }
        if(!found_in_frame){
            print_frame(frame,F,f_itr,smallest,not_full,found_in_frame,page_fault);
            page_fault.push_back(true);
            page_fault_counter++;
            
        }else{
            page_fault.push_back(false);
            
        }
        f_itr++;
    }
    std::cout << "End of LFU simulation ("<< page_fault_counter <<" page faults)"<< std::endl;
    
}

