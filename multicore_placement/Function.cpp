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
//  File: Function.cpp

#include "Function.h"
#include <algorithm>

// The max number of locations for each feta

// This constructor randomly generate the function structure
Function::Function(std::string n)
{
    name = n;
    Block* block;
    float N = 4 * (float)rand() / (float)RAND_MAX;
    
    // Generate the FETA
    int n_loc = (int)(1 + ( N * (float)rand() / (float)RAND_MAX) );
    for (auto i = 0; i < n_loc; ++i)
    {
        block = new Block();
        blocks.push_back(block);
        computeFeta();
    }
    
    // I try to reduce the size of the functions. I don't need blocks anymore.
    blocks.clear();
    
    std::vector<int> base_periods;
    base_periods.push_back(1);
    base_periods.push_back(2);
    base_periods.push_back(3);
    base_periods.push_back(4);
    //base_periods.push_back(5);
    //base_periods.push_back(7);
    //base_periods.push_back(9);
    
    /**
     Generate the period randomly, by choosing it between one of the values
     in base_periods
     */
    float M = 2 * (float)rand() / (float)RAND_MAX;
    int NN = 1 + int(M * ((float)rand() / (float)RAND_MAX));
    period = 1;
    for (int i = 0; i < NN; i++)
    {
        int idx = (int)(base_periods.size() * (float)rand() / (float)RAND_MAX);
        period = period * base_periods[idx];
    }
}

Function::Function(std::string n, long p, std::vector<float> fta)
{
    name = n;
    period = p;
    places = fta;
}

Function::~Function()
{
    
}

void Function::setFeta(std::vector<float> p)
{
	places = p;
}

long Function::getLinkSize(Function *f)
{
    if (f == nullptr)
        return -1;
    
    if (successors.find(f) != successors.end())
        return successors.at(f);
    else if (predecessors.find(f) != predecessors.end())
        return predecessors.at(f);
    else
        return -1;
}

void Function::computeFeta()
{
    if (blocks.size() == 0) return;
    
    // Compute Period
    period = blocks[0]->getPeriod();
    for (auto i = blocks.begin(); i != blocks.end(); i++)
        period = gcd(period, (*i)->getPeriod());
    
    // Compute Hyperperiod
    long hyperperiod = (blocks[0]->getPeriod());
	for (auto i = blocks.begin(); i != blocks.end(); i++)
		hyperperiod = lcm(hyperperiod, (*i)->getPeriod());
    
    places.clear();
    for (auto i = 0 ; i < hyperperiod/period; i++)
        places.push_back(0);
    
    for (auto i = places.begin(); i != places.end(); i++)
    {
        float wcet = 0;
        long t = (i - places.begin()) * period;
        
        for (auto j = blocks.begin(); j != blocks.end(); j++)
            wcet += (*j)->getFeta(t);
        
        (*i) = wcet;
    }
}

std::string Function::getName()
{
	return name;
}

long Function::getHyperperiod()
{
	return period * places.size();
}

const std::map<Function*, long>& Function::getPredecessors()
{
    return predecessors;
}

const std::map<Function*, long>& Function::getSuccessors()
{
    return successors;
}

void Function::addPredecessor(Function *f, long bus_size)
{
    predecessors[f] = bus_size;
}

void Function::addSuccessor(Function *f, long bus_size)
{
    successors[f] = bus_size;
}

void Function::removePredecessor(Function * f)
{
    predecessors.erase(f);
}

void Function::removeSuccessor(Function * f)
{
    successors.erase(f);
}


void Function::setUtilizationBound(float ub)
{
    auto norm = places[0];
    for (auto i = places.begin(); i != places.end(); i++)
    {
        auto time = ( i - places.begin() ) * period;
        
        // Normalize the FETA and set its Utilization Bound
        (*i) = ((*i) / norm) * ub * dstNext(time);
    }
    

    
}

void Function::print(std::ostream& stream)
{
    stream << "  + Function: " << name << " - Period: " << period << std::endl;
    stream << "    - ";
    for (auto i = places.begin(); i != places.end(); i++)
        stream << "[" << (*i) << "] ";
    stream << std::endl;
}

void Function::printBlocks(std::ostream& stream)
{
    for (auto block = blocks.begin(); block != blocks.end(); block++)
        std::cout << "  --- Block period: " << (*block)->getPeriod() << std::endl;
}


void Function::add(Feta *f)
{
    if (dynamic_cast<Block*>(f))
    {
        Block* block = dynamic_cast<Block*>(f);
        blocks.push_back(block);
        computeFeta();
    }
}

void Function::remove(Feta *f)
{
    
}








