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

//  File: Task.h

#ifndef __tbd_framework__Task__
#define __tbd_framework__Task__

#include <iostream>
#include <string>
#include <vector>
#include "Feta.h"
#include "Function.h"
#include "Extras.h"

class Task : public Feta{
    std::string name;
    long priority;
    std::vector<Function* > functions;
    
    void computeFeta();
    
public:
    
    Task();
    Task(std::string, long, std::vector<Function* >&);
    Task(std::string, long);
    ~Task();
    Task(const Task& other);
	Task& operator = (const Task& other);
    
    bool isMy(Function* );
    
    void remove(Feta* );
    bool removeC(Feta* );
    void remove(std::vector<Function*>* );
    void add(Feta* );
    void add(std::vector<Function* >* );
    
    long getPriority();
    void changePriority(long);
    
    void print(std::ostream&);
    void printExt(std::ostream&);
    
    const std::vector<Function* >& getFunctions();
    std::vector<Function* > getPredecessors();
    
    long getFunSize();
    
    std::string getName() {return name;}

    long getP();
    
    float getWcet();
    
    void getFunctionSet(std::vector<Function* >*, int);
    
    void replaceFunctions(std::vector<Function*>);
};


#endif /* defined(__tbd_framework__Task__) */
