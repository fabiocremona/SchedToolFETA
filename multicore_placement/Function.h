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
//  File: Function.h

#ifndef __tbd_framework__Function__
#define __tbd_framework__Function__

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "Feta.h"
#include "Block.h"
#include "Extras.h"

 
class Function: public Feta {
    
    std::vector<Block*> blocks;
    
    std::map<Function*, long> predecessors;
    std::map<Function*, long> successors;
    
    std::string name;
    
    void computeFeta();
    
public:
    Function(std::string);
    Function(std::string, long, std::vector<float>);
	~Function();
    
    void setUtilizationBound(float);
    
    std::string getName();
    long getHyperperiod();

    const std::map<Function*, long>& getPredecessors();
    const std::map<Function*, long>& getSuccessors();
    const std::vector<Block*> getBlocks() {return blocks;};
    
    void addSuccessor(Function*, long);
    void addPredecessor(Function*, long);
    void removeSuccessor(Function*);
    void removePredecessor(Function*);
    
    void print(std::ostream&);
    void printBlocks(std::ostream&);
    
    long getSizePreds() {return predecessors.size();};
    long getSizeSuccs() {return successors.size();};
    
    void add(Feta*);
    void remove(Feta*);
};



#endif /* defined(__tbd_framework__Function__) */
