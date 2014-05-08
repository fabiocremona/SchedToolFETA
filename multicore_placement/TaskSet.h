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
#include "Processor.h"

class Processor;
class TaskSet : public Feta {
    
    long last_link_size;
    std::vector<std::pair<Function*, Function*>> last_links;
    
    std::vector<Task* > taskset;
    std::vector<Function*> functions;
    std::vector<float> rbf;
    std::vector<std::pair<std::vector<float>, long>> rbfs;
    std::vector<float> response_t;
    std::vector<float> slaks;
    long total_size;
    long rt2_size;
    long rt1_size;
    void computeRbf();
    bool isEmpty();
    std::map<Function*, long> offsets;
    
    void refreshFunctions();
    
    // ====> Here below, is for optimization <====
    
    // ---> [[src_function-dst_function], size]
    std::map<std::pair<Function*, Function*>, long> rt2s;
    Task* src_task;
    Task* dst_task;
    
    std::vector<Function*> src_funs;
    std::vector<Function*> dst_funs;    
    
public:
    
    TaskSet();
    TaskSet(std::vector<Task*>&);
    ~TaskSet();
    TaskSet(const TaskSet& other);
	TaskSet& operator = (const TaskSet& other);
    
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
        
    // Add a function to the taskset
    void addFunction(Function*, float);
    
    float addFunction(Function*, Processor*);
    void remove(Feta *);
    void removeFunction(Function*);
        
    // Compute the FETA and returns the RBF
    void computeFeta();
    
    // Returns the RBF value at any given point in time
    float getRbf(long);
    
    std::vector<float> getRbf();
    
    // Check the TaskSet schedulability
    // it returns TRUE if the TaskSet is schedulable, FALSE otherwise
    bool checkSchedulability();
    
    // Compute the response time for taskset with offset
    bool computeResponseTimeO();
    
    // Returns the response time of a specified runnable
    float getResponseTime(Function* );
    
    // Returns the response time + offset of a specified runnable
    float getCompletionTime(Function * );
    
    // Returns the response time of all tasks
    std::vector<float> getResponseTimes();
    
    // Returns the slacks of each task
    std::vector<float> getSlack();
    
    // Returns the minimum slack between those of all the tasks
    float getMinSlack();
    
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

    float getLoad();
    
    // ====> Here below, is for optimization <====
    
    void optimize(Function*, Function*);
    
    void undoOptimize();
    void confirmOptimization();
    
    // Returns the link with higher RT2 size
    std::pair<Function*, Function*> getLink();
    
    //Peng:
	// Randomly change task set priorities
	void randomSwapPriorities();
	
	// Randomly partition task set
	void randomPartition();
	
	// Randomly merge task sets
	void randomMerge();
    
    long getTaskSetSize();
    
    // Compute the synchronous set for a given runnable.
    // You must give two parameters:
    // - std::vector<Function*>* : the container for the synchronous set
    // - Function * : the runnable for which you want compute the synchronous set
    void getSyncSet(std::vector<Function*>*, Function *);
};
#endif /* defined(__tbd_framework__TaskSet__) */
