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

//  File: RtwOpt.h

#ifndef __fetalib__RtwOpt__
#define __fetalib__RtwOpt__

#include <iostream>
#include <vector>
#include <map>
#include "TaskSet.h"

class RtwOpt {
    
    TaskSet original_ts;
    TaskSet task_set;
    
    // ---> [[src_function-dst_function], size]
    std::map<std::pair<Function*, Function*>, long> rt2s;
    
    // For each runnable scan the list of successors and create rt2s map
    // it returns the total size of RT2 blocks
    long computeRTB2();
    
    // Compute the synchronous set for a given runnable.
    // You must give two parameters:
    // - std::vector<Function*>* : the container for the synchronous set
    // - Function * : the runnable for which you want compute the synchronous set
    void getSyncSet(std::vector<Function*>*, Function *);
    
    // Returns the task where you can put the runnable (the sink) to remove
    // the RT2 block
    // TODO: First note: why is it only one task? It can be a set of tasks!
    Task* getOptTask(Function*);
    
    // Returns the link with higher RT2 size
    std::pair<Function*, Function*> getLink();
    
    
public:
    RtwOpt(TaskSet&);
    ~RtwOpt();
    
    std::vector<Function*> getSS(Function*);
    
    TaskSet optimize();
    
    long getRT2();
};

#endif /* defined(__fetalib__RtwOpt__) */
