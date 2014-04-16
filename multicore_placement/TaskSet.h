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

//  File: TaskSet.h

#ifndef __tbd_framework__TaskSet__
#define __tbd_framework__TaskSet__

#include <iostream>
#include <vector>
#include <map>
#include <fstream>

#include "Feta.h"
#include "Task.h"
#include "Extras.h"

class TaskSet : public Feta {
    
    std::vector<Task* > taskset;
    std::vector<float> rbf;
    
    std::vector<std::pair<std::vector<float>, long>> rbfs;
    std::vector<float> response_t;
    std::vector<float> slaks;
    
    long total_size;
    long rt2_size;
    long rt1_size;
    
    bool checkInterval(long, long);
    void computeRbf();
    bool isEmpty();
    
    // Compute the response time for allo the tasks in the taskset based on rbfs
    // It called by checkSchedulability() in case the task set is schedulable
    // NB: it is used oncly for offset free tasks
    void computeResponseTime();
    
    
    
public:
    
    TaskSet();
    TaskSet(std::vector<Task*>&);
    ~TaskSet();
    
    std::vector<Task* >& getTs();
    
    // Returns the priority of the task where the function is executing
    long getPriority(Function* );
    Task * getTask(long);
    Task * getTask(Function*);
    
    void printTs(std::ostream&);
    void printTsExt(std::ostream&);
    void printRbfs(std::ostream&);
    
    void add(Feta *);
    void addFunction(Function*);
    void addFunction(Function*, float);
    void remove(Feta *);
    void removeFunction(Function*);
    
    void moveFunctions(std::vector<Function* > *, Task *);
    
    // Compute the FETA and returns the RBF
    void computeFeta();
    
    // Returns the RBF value at any given point in time
    float getRbf(long);
    
    std::vector<float> getRbf();
    
    // Check the TaskSet schedulability
    // it returns TRUE if the TaskSet is schedulable, FALSE otherwise
    bool checkSchedulability();
    
    // Compute the response time for taskset with offset
    float computeResponseTimeO();
    
    // Returns the response time of a specified runnable
    float getResponseTime(Function* );
    
    // Returns the response time of all tasks
    std::vector<float> getResponseTimes();
    
    // Returns the slacks of each task
    std::vector<float> getSlack();
    
    // Returns the minimum slack between the slacks of all the tasks
    float getMinSlack();
    
    // Return the set of affine tasks of task t
    std::vector<Task*> getAffineTasks(Task* t);
    
    // Check if a task is allocated on this TaskSet
    bool isMy(Task*);
    
    // Check if a runnable is allocated on this TaskSet
    bool isMy(Function*);
    
    // Set the priorities according to the RM policy
    void setRMPriorities();
    
    // Re-arrange task set priorities
    void adjustPriorities();
    
    // Compute the toal amount of memory required by RT blocks (type 1 and 2)
    // for intra core
    void computeRT();
    
    // get the total amount of memory required by RT blocks of type 1
    long getRT1();
    
    // get the total amount of memory required by RT blocks of type 2
    long getRT2();
    
    // fet the total amount of memory required by RT blocks
    long getRT();
    
    // This test compute the response time of the task set considering inter core
    // synchronization
    bool schedTest();

};
#endif /* defined(__tbd_framework__TaskSet__) */
