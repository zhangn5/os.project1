//
//  main.cpp
//  project2
//
//  Created by Ni Zhang on 12/2/16.
//  Copyright © 2016 Ni Zhang. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include "VM.h"


int main(int argc, const char * argv[]) {
    if(argc != 2){
        std::cerr << "USAGE: "<< argv[0]<< "<input-file>" << std::endl;
        return 1;
    }
    std::ifstream page_str(argv[1]);
    if(!page_str){
        std::cerr << "Can not open the page file " << argv[1] << std::endl;
        return 1;
    }
    VM pages_in(page_str);
    pages_in.OPT();
    std::cout << std::endl;

    pages_in.LRU();
    std::cout << std::endl;

    pages_in.LFU();

}
