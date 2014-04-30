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
#include <map>

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
    
    // Return the list of already allocated runnables
    std::vector<Function*> getAllocatedRunnables();
    
    // Returns the response time for each allocated runnable
    std::map<Function*, float> getResponseTimes();
    
    bool checkSchedulability(std::map<Function*, float>&);
    
    bool checkSchedulability();
    
    // Returns the offsets of all the tasks in all the tasksets
    std::map<Task*, long> getOffsets();
    
    // Compute the offsets for all tasks with respect to the allocated
    // runnables
    std::map<Task*, float> getNewOffsets();
    
    // Set the new offsets
    std::map<Task*, float> setNewOffsets();
    
    void setOffsets(std::map<Function*, float>);
    
    // Set all offsets to zero
    void clearOffsets();
    
    // Returns the set of cores which contains at list one task
    std::vector<TaskSet*> getAllocCores();
    
    // Returns the offset for a given runnable
    std::pair<TaskSet*, float> getOffset(Function*);
    
    
    
public:
    
    Processor(std::vector<Task* > , int);
    Processor(std::vector<Function*>, int);
    ~Processor();
    
    // Allocate runnables with an utilization bound for each core
    bool interCoreAllocation(float);
    
    // Optimize RT blocke of type 2
    void optimizeRT2();
    
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
