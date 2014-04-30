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
// #cores, Ub

int main( int argc, const char*   argv[] )
{
    // number of cores
    //int NC = 4;
    int NI = 10;
    
    std::string nc = argv[1];
    int NC = (int)atof(nc.c_str());
    
    std::string ub = argv[2];
    int UB = (int)atof(ub.c_str());
    
    ofstream output;
    output.open("results.txt");
    
    // enable random seed
    srand(unsigned(time(NULL)*99999999999999));
    
    for (auto u = 0.1; u <= NC; u += 0.1)
    {
        int occurs = 0;
        
        for (auto i = 0; i < NI; i++)
        {
            system("./tgff test_graph");
            Parser ps0("test_graph.tgff", to_string(u));
            ps0.create();
            
            output << "Iteration: " << i << " load: " << u << std::endl;
            std::cout << "Iteration: " << i << " load: " << u << std::endl;
            
            output << "Functions: " << std::endl;
            for (auto f : ps0.getFunctions())
                f->print(output);
            
            Processor p0(ps0.getFunctions(), NC);
            bool is_sched = p0.interCoreAllocation(UB);
            
            if (is_sched == true)
            {
                output << "Solution schedulable" << std::endl;
                p0.print(output);
                occurs++;
            }
            else
                output << "Solution NOT schedulable" << std::endl;
        }
        output << "===> Schedulability: " << occurs/NI*100 << "%" << std::endl;
        std::cout << "===> Schedulability: " << occurs/NI*100 << "%" << std::endl;
    }
    return 0;
}



