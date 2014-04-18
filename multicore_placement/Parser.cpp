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
//  File: Parser.cpp

#include "Parser.h"
#include <stdlib.h>
#include <algorithm>

Parser::Parser(std::string file)
{
    source_file = file;
    Up = (double)rand() / (double)RAND_MAX;
    Up = 0.8 + (-0.15 + Up * 0.3);
    tasks = new std::vector<Task* >;
}


Parser::Parser(std::string file, std::string up)
{
    source_file = file;
    Up = (float)atof(up.c_str());
    tasks = new std::vector<Task* >;
}

Parser::~Parser()
{
    for (auto component : components)
        delete component;
    for (auto function : functions)
        delete function.second;
}

void Parser::create()
{
    std::ifstream source;
    std::string line;
    
    try {
        source.open(source_file.c_str());
        
    } catch (...) {
        throw "Unable to open source file";
    }
    
    while (source.good()) {
        std::string  type, name;
        getline(source, line);
        std::istringstream linestream(line);
        linestream >> type;
        
        if (type == "@TASK_GRAPH") {
            linestream >> name;
            Component* comp_tmp = new Component(name);
            components.push_back(comp_tmp);
        }
        if (type == "TASK") {
            linestream >> name;
            Function* fun_tmp = new Function(name);
            functions[name] = fun_tmp;
            components.back()->addFunction(fun_tmp);
        }
        if (type == "ARC") {
            std::string from, to;
            linestream >> from >> from >> from;
            linestream >> to >> to;
            components.back()->connect(functions[from], functions[to]);
        }
    }
    source.close();
    
    // Set components utilization
    auto n_fun = functions.size();
    for (auto i = functions.begin(); i != functions.end(); i++)
        (*i).second->setUtilizationBound(Up / n_fun);
    
    checkFunctions();
}

std::vector<Task* > * Parser::getRMTS()
{
    std::map<long, std::vector<Function *>* > tmp_tasks;
    
    // Scann all components
    for (auto i = components.begin(); i != components.end(); i++) {
        
        // Scann all functions in each component
        for (auto j = (*i)->getFunctions().begin();
                            j != (*i)->getFunctions().end(); j++) {
            //(*j).second->print(std::cout);
            auto k = tmp_tasks.find((*j).second->getPeriod());
            if (k != tmp_tasks.end())
                k->second->push_back((*j).second);
            else {
                std::vector<Function* >* tmp0= new std::vector<Function *>;
                tmp0->push_back((*j).second);
                tmp_tasks[(*j).second->getPeriod()] = tmp0;
            }
        }
    }
    
    // Create Tasks
    auto p = 1;
    for (auto i = tmp_tasks.begin(); i != tmp_tasks.end(); i++) {
        std::string tmp_name;
        
        // Create the name of the task as sum of the name of its functions
        for (auto j = (*i).second->begin(); j != (*i).second->end(); j++)
            tmp_name+= (*j)->getName();
        auto pr = p++;
        Task* tmp = new Task(tmp_name, pr, *(*i).second);
        tasks->push_back(tmp);
    }
    return tasks;
}

void Parser::checkFunctions()
{
    for (auto f : functions)
    {
        auto function = f.second;
        auto feta = function->get();
        
        auto startvalue = feta[0];
        for (auto i : feta)
        {
            if (i > startvalue) {
                std::cout << "ERROR: function " << function->getName()
                << " has a bad FETA" << std::endl;
                exit(-1);
            }
        }
    }
}


