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

//  File: Parser.h

#ifndef __fetalib__Parser__
#define __fetalib__Parser__

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>

#include "Function.h"
#include "Component.h"
#include "Task.h"

class Parser {
    std::string source_file;
    std::string graph_file;
    std::string feta_file;
    
    std::vector<Component *> components;
    std::map<std::string, Function*> functions;
    std::vector<Task *> *tasks;
    void checkFunctions();
    
    float Up;
public:
    Parser(std::string);
    Parser(std::string, std::string);
    ~Parser();
    void create();
    std::map<std::string, Function*> getFunctions() {return functions;};
    std::vector<Task* > * getRMTS();
};



#endif /* defined(__fetalib__Parser__) */
