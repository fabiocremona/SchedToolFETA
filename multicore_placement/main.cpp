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

#define MULTIPLIER 99999999999999


int main( int argc, const char*   argv[] )
{
    ofstream output;
    output.open("test_0_results.txt");
    //srand(unsigned(time(NULL)*99999999999999));
Parser ps0("/Users/fabiocremona/Documents/SSSA-PhD/Works/FETA/multicore_placement/multicore_placement/test_1.txt", "3");
    ps0.create();
    TaskSet ts0(*ps0.getRMTS());
    
//    auto is_schedulable = ts0.checkSchedulability();
//    if (!is_schedulable) {
//        std::cout << "The task set is not schedulable" << std::endl;
//        exit(-1);
//    }
    
//    RtwOpt opt0(ts0);
    //ts0.printTs(std::cout);
//    auto ts1 = opt0.optimize();
    //ts1.printTs(std::cout);
    
    ts0.printTsExt(std::cout);
    std::cout << std::endl;
    
    Processor p0(ts0.getTs(), 5);
    p0.interCoreAllocation(0.8);
    p0.print(std::cout);
    
    std::cout << "End" << std::endl;
    
    return 0;
}



