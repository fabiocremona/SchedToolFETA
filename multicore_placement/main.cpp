/*
 F-Sched: Scheduling, Optimization and Placement of AUTOSAR Runables
 Copyright (C) <2014>  <Fabio Cremona> fabiocremona@gmail.com
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//  main.cpp

#include <iostream>
#include <fstream>


#include "Parser.h"
#include "TaskSet.h"
#include "RtwOpt.h"
#include "Processor.h"

#include <stdio.h>
#include <stdlib.h>

#define MULTIPLIER 99999999999999

// Arguments:
// - input file
// - load
// - number of cores
// - ub
// - Random <R>

int main( int argc, const char*   argv[] )
{
    ofstream output;
    output.open("test_0_results.txt");
    if (*argv[5] == 'R')
        srand(unsigned(time(NULL)*99999999999999));
    

    bool is_sched;
    int occurs = 0;
    int iter_num = (int)atof(argv[6]);
    
    for (auto i = 0; i < iter_num; i++)
    {
        output << "Iteration: " << i << std::endl;
        std::string tgff_command = "./tgff ";
        tgff_command = tgff_command + argv[1];
        system("./tgff tg");
        Parser ps0(argv[1], argv[2]);
        ps0.create();
        TaskSet ts0(*ps0.getRMTS());
        
        std::string n_cores = argv[3];
        std::string ub = argv[4];
        
        Processor p0(ts0.getTs(), (int)atof(n_cores.c_str()));
        is_sched = p0.interCoreAllocation((float)atof(ub.c_str()));
        
        if (is_sched == true)
            occurs++;
        
        p0.print(output);
        output << std::endl;
        std::cout << "Done: " << i << std::endl;
    }
    
    std::cout << "Schedulability: " << occurs/iter_num << "%" << std::endl;
    
    return 0;
}



