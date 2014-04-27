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

//  File: Task.cpp

#include "Task.h"
#include <algorithm>

Task::Task()
{
    offset = 0;
}

Task::Task(std::string n, long p, std::vector<Function* >& f)
{
    offset = 0;
    name = n;
    priority = p;
    for (auto i = f.begin(); i != f.end(); i++)
    {
        functions.push_back(*i);
        computeFeta();
    }
}

Task::Task(std::string n, long p)
{
    offset = 0;
    name = n;
    priority = p;
}

Task::~Task()
{
    
}

void Task::computeFeta()
{
    if (functions.size() == 0)
    {
        places.clear();
        return;
    }
    
    // Compute Period
    period = functions[0]->getPeriod();
    for (auto i = functions.begin(); i != functions.end(); i++)
        period = gcd(period, (*i)->getPeriod());

    // Compute Hyperperiod
    long hyperperiod = (functions[0]->getHyperperiod());
	for (auto i = functions.begin(); i != functions.end(); i++)
		hyperperiod = lcm(hyperperiod, (*i)->getHyperperiod());
    
    places.clear();
    for (auto i = 0 ; i < hyperperiod/period; i++)
        places.push_back(0);
    
    
//    if (offset != 0)
//        period = gcd(period, offset);
//    for (auto i = 0; i < (offset/period); i++)
//        places.push_back(0);
    
    for (auto i = places.begin(); i != places.end(); i++)
    {
        float wcet = 0;
        long t = (i - places.begin()) * period;
        for (auto j = functions.begin(); j != functions.end(); j++)
            wcet += (*j)->getFeta(t);
        
        (*i) = wcet;
    }
}

bool Task::isMy(Function *f)
{
    auto i = std::find(functions.begin(), functions.end(), f);
    if (i != functions.end())
        return true;
    return false;
}

void Task::remove(Feta *f)
{
    if (dynamic_cast<Function*>(f))
    {
        Function *fun = dynamic_cast<Function*>(f);
        auto i = std::find(functions.begin(), functions.end(), fun);
        if (i != functions.end())
        {
            auto tmp_name = fun->getName() + "t";
            auto tmp0 = name.find(tmp_name);
            if (tmp0 == std::string::npos)
                tmp0 = name.rfind(fun->getName());
            auto tmp1 = fun->getName().size();
            
            name.erase(tmp0, tmp1);
            functions.erase(i);
            computeFeta();
        }
    }
    
}

bool Task::removeC(Feta *f)
{
    if (dynamic_cast<Function*>(f))
    {
        Function *fun = dynamic_cast<Function*>(f);
        auto i = std::find(functions.begin(), functions.end(), fun);
        if (i != functions.end())
        {
            
            auto tmp_name = fun->getName() + "t";
            auto tmp0 = name.find(tmp_name);
            if (tmp0 == std::string::npos)
                tmp0 = name.rfind(fun->getName());
            auto tmp1 = fun->getName().size();
            
            name.erase(tmp0, tmp1);
            functions.erase(i);
            computeFeta();
            return true;
        }
    }
    
    return false;
}

void Task::remove(std::vector<Function*> *f)
{
    for (auto i = f->begin(); i != f->end(); i++)
        remove(*i);
}

void Task::add(Feta *f)
{
    if (dynamic_cast<Function*>(f))
    {
        Function *fun = dynamic_cast<Function*>(f);
        auto i = std::find(functions.begin(), functions.end(), fun);
        if (i == functions.end())
        {
            functions.push_back(fun);
            name = name + fun->getName();
            computeFeta();
        }
    }
}

void Task::add(std::vector<Function* >* f)
{
    for (auto i = f->begin(); i != f->end(); i++)
        add(*i);
}

long Task::getPriority()
{
    return priority;
}

void Task::changePriority(long p)
{
    priority = p;
}

void Task::print(std::ostream &stream)
{
    stream << "TASK: " << name << std::endl;
    stream << " - Priority: " << priority << " - Period: " << period
    << " - Offset: " << offset << std::endl;
    stream << " - ";
    for (auto i = places.begin(); i != places.end(); i++)
        stream << "[" << (*i) << "] ";
    stream << std::endl;
    
}

void Task::printExt(std::ostream &stream)
{
    stream << "TASK: " << name << std::endl;
    stream << " - Priority: " << priority << " - Period: " << period
    << " - Offset: " << offset << std::endl;
    stream << " - ";
    for (auto i = places.begin(); i != places.end(); i++)
        stream << "[" << (*i) << "] ";
    stream << std::endl;
    
    for (auto i = functions.begin(); i != functions.end(); i++)
        (*i)->print(stream);
    
}

const std::vector<Function* >& Task::getFunctions()
{
    return functions;
}

std::vector<Function* > Task::getPredecessors()
{
    std::vector<Function *> predecessors;
    for (auto function : functions)
        for (auto pred : function->getPredecessors())
            predecessors.push_back(pred.first);
    return predecessors;
}


long Task::getFunSize()
{
    return functions.size();
}

long Task::getP()
{
    auto p = functions.back()->getPeriod();
    for (auto f : functions)
        p = gcd(p, f->getPeriod());
    return p;
}

float Task::getWcet()
{
    float wcet = 0;
    for (auto f : functions)
        wcet += f->getFeta(0);
    return wcet;
}
