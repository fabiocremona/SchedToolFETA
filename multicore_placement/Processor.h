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

//  File: Processor.cpp.h

#ifndef __feta_test__Processor__
#define __feta_test__Processor__

#include <iostream>
#include <vector>

#include "Task.h"
#include "TaskSet.h"

using namespace std;

class TaskSet;

class Processor {
    
    TaskSet *task_set;
    
    std::vector<Function*> runnables;
    
    std::vector<Function*> NAR;
    
    std::vector<TaskSet*> cores;
    
    long total_size;
    long rt2_size;
    long rt1_size;
    
    void orderByDensity(std::vector<Task*>);
    void orderByDensity(std::vector<Function*>);
    
    // Returns the list of affine cores wich utiliazation bound is below
    //  a given threshold
    std::vector<TaskSet*> getAffineCores(Function*, float);
    
    // Returns the list of runnables which has predecessors already allocated
    std::vector<Function*> getEnabledRunnables();
    
    // Print some internal stats
    void printInternalStats(std::vector<TaskSet*>);
        
public:
    
    Processor(std::vector<Task* > , int);
    Processor(std::vector<Function*>, int);
    ~Processor();
    
    // Allocate runnables with an utilization bound for each core
    void interCoreAllocation(float);
    
    void test();
    
    long getCoreIdx(Function*);
    TaskSet* getCore(Function*);
    void print(std::ostream&);
    
    // Compute the toal amount of memory required by RT blocks (type 1 and 2)
    // for inter core
    void computeRT();
    
    // get the total amount of memory required by RT blocks of type 1
    long getRT1();
    
    // get the total amount of memory required by RT blocks of type 2
    long getRT2();
    
    // fet the total amount of memory required by RT blocks
    long getRT();
};




#endif /* defined(__feta_test__Processor__) */
