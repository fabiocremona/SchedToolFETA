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
//  File: Component.cpp

#include "Component.h"
#include <algorithm>

Component::Component(std::string n)
{
    name = n;
}

Component::~Component()
{
    
}


void Component::addFunction(Function* f)
{
    functions[f->getName()] = f;
}

void Component::connect(Function* src, Function* dst)
{
    // Set the bus size
    // Random number (Byte) 10 - 1000
    long bus_size = (long)(10 + 990 * ((float)rand() / (float)RAND_MAX));
    
    functions[src->getName()]->addSuccessor(dst, bus_size);
    functions[dst->getName()]->addPredecessor(src, bus_size);
}

void Component::connect(Function* src, Function* dst, long bus_size)
{
    functions[src->getName()]->addSuccessor(dst, bus_size);
    functions[dst->getName()]->addPredecessor(src, bus_size);
}

void Component::setUtilizationBound(float uc)
{
    for (auto i = functions.begin(); i != functions.end(); i++)
        (*i).second->setUtilizationBound(uc / functions.size());
}

std::map<std::string, Function *>& Component::getFunctions()
{
    return functions;
}


