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
    Parser ps0(argv[1], argv[2]);
    
    ps0.create();
    TaskSet ts0(*ps0.getRMTS());
    
    std::string n_cores = argv[3];
    std::string ub = argv[4];
    Processor p0(ts0.getTs(), (int)atof(n_cores.c_str()));
    p0.interCoreAllocation((float)atof(ub.c_str()));

    p0.print(std::cout);
    p0.print(output);
    
    std::cout << "End" << std::endl;
    
    return 0;
}



