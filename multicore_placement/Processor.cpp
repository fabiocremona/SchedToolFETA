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

//  File: Processor.cpp

#include "Processor.h"
#include <algorithm>

bool sort_density(Task* a, Task* b)
{
    return (( (a)->get()[0] / (a)->getPeriod() ) >
            ( (b)->get()[0] / (b)->getPeriod() ));
}

bool sort_Fdensity(Function* a, Function* b)
{
    return (( (a)->get()[0] / (a)->getPeriod() ) >
            ( (b)->get()[0] / (b)->getPeriod() ));
}

Processor::Processor(std::vector<Task* >  ts, int n_cores)
{    
    std::sort(ts.begin(), ts.end(), sort_density);
    
    for (auto i = 0 ; i < n_cores; i++)
        cores.push_back(new TaskSet());
    
    for (auto task: ts)
        for (auto f : task->getFunctions())
            runnables.push_back(f);
    NAR = runnables;
}

Processor::Processor(std::vector<Function*> runnables, int n_cores)
{
    for (auto i = 0 ; i < n_cores; i++)
        cores.push_back(new TaskSet());
    NAR = runnables;
}

Processor::~Processor()
{
    
}

void Processor::orderByDensity(std::vector<Task*> v)
{
    std::sort(v.begin(), v.end(), sort_density);
}

// Returns the list of all affine cores.
std::vector<TaskSet*> Processor::getAffineCores(Function* f, float Ub)
{
    std::vector<TaskSet*> affine;
    
    for (auto function : f->getPredecessors())
        for (auto core : cores)
            for (auto task : core->getTs())
                if (task->isMy(function.first) && core->getLoad() < Ub)
                    affine.push_back(core);

    if (affine.size() == 0) return cores;
    
    return cores;
    //    else
    //    {
    //        // Remove duplicate cores
    //        std::sort(affine.begin(), affine.end());
    //        auto it = std::unique(affine.begin(), affine.end());
    //        affine.resize(std::distance(affine.begin(), it));
    //        
    //        std::vector<TaskSet* > ok_cores;
    //        for (auto core : affine)
    //        {
    //            if (core->getLoad() < Ub)
    //                ok_cores.push_back(core);
    //        
    //            std::cout << "CPU Load: " << core->getLoad() << std::endl;
    //        }
    //        return ok_cores;
    //    }
}

std::vector<Function*> Processor::getEnabledRunnables()
{
    std::vector<Function*> enabled_runnables;
    
    for (auto runnable : NAR)
    {
        auto affine_set = runnable->getPredecessors();
        
        bool en_cond = true;
        for (auto a_runn : affine_set)
        {
            auto i = std::find(NAR.begin(), NAR.end(), a_runn.first);
            
            if (i != NAR.end())
                en_cond = false;
        }
        
        if (en_cond)
            enabled_runnables.push_back(runnable);
    }
    
    return enabled_runnables;
}

void Processor::interCoreAllocation(float Ub)
{
    std::vector<Function*> BL;
    std::vector<Function*> PBL;
    
    while (NAR.size() != 0)
    {
        // Take the enabled tasks and order them by density
        auto en_runnables = getEnabledRunnables();
        std::sort(en_runnables.begin(), en_runnables.end(), sort_Fdensity);
        auto runnable = en_runnables.front();
        
        std::cout << "Trying to allocate: " << runnable->getName() << std::endl;
        
        // Get affine cores
        std::vector<TaskSet*> affine_cores;
        if (std::find(PBL.begin(), PBL.end(), runnable) == PBL.end())
            affine_cores = getAffineCores(runnable, Ub);
        else affine_cores = cores;
        
        std::cout << "Affine cores " << affine_cores.size() << std::endl;
        
        // Get the response time for all affine runnables
        std::vector<float> offsets;
        offsets.clear();
        offsets.push_back(0);
        
        // Compute the initial offset for the runnable
        for (auto core : affine_cores)
        {
            for (auto pred : runnable->getPredecessors())
            {
                float offset = core->getCompletionTime(pred.first);
                
                if (offset > 0)
                {
                    std::cout << "Predecessor: " << pred.first->getName() <<
                    " on core " << getCoreIdx(pred.first) << std::endl;
                    std::cout << "\tOffset: " << offset << std::endl;
                    offsets.push_back(offset);
                }
                
                else if (offset == 0)
                    continue;
                else if (offset == -1)
                {
                    std::cout << "Error: Previous taskset not schedulable" <<
                    std::endl;
                    exit(-1);
                }
            }
        }
        
        float offset = (*std::max_element(offsets.begin(), offsets.end()));
        long core_idx = offsets.begin() - std::find(offsets.begin(),
                                                    offsets.end(),
                                                    offset);
        
        
        // Allocate runnable on all affine cores (tryAssign)
        std::map<TaskSet*, float> affine_sched_cores;
        for (auto core : affine_cores)
        {
            auto worst_core = affine_cores[core_idx];
            // In case I am allocating the runnable in the same core of the
            //  predecessor with the worst response time, I don't need to
            // allocate with offset
            if (core == worst_core)
                core->addFunction(runnable);
            
            // Otherwise, allocate with offset
            else
                core->addFunction(runnable, offset);
            
            // Check the schedulability for the allocated function in each core
            if(core->checkSchedulability() == true)
                affine_sched_cores[core] = core->getMinSlack();

            // If the core is not schedulable, remove the assignment
            else
                core->removeFunction(runnable);
        }
        
        std::cout << "Affine schedulable cores " << affine_sched_cores.size() << std::endl;
        //printInternalStats(cores);
        
        // If there are schedulable affine solutions
        //  - take the best allocation
        //  - remove other assignments
        if (affine_sched_cores.size() != 0)
        {
            // Find the allocation that maximize the minimum slack
            TaskSet* best_core = affine_sched_cores.begin()->first;
            for (auto i : affine_sched_cores)
                if(i.second > affine_sched_cores[best_core]) best_core = i.first;
            
            std::cout << "Runnable " << runnable->getName() << " allocated on core " << std::distance(cores.begin(), std::find(cores.begin(), cores.end(), best_core)) << " with offset: " << best_core->getTask(runnable)->getOffset() << std::endl;
            
            // Remove all the allocations except the best
            for (auto i = cores.begin(); i != cores.end(); i++)
                if (*i != best_core) (*i)->removeFunction(runnable);
            
            
            // Eventually remove "runnable" from pbl and bl lists
            auto pbl = std::find(PBL.begin(), PBL.end(), runnable);
            auto bl = std::find(BL.begin(), BL.end(), runnable);
            
            if (pbl != PBL.end())
                PBL.erase(pbl);
            
            if (bl != BL.end())
                BL.erase(bl);
            
            // Remove "runnable" from those runnables to allocate
            NAR.erase(std::find(NAR.begin(), NAR.end(), runnable));
        }
        
        else if (affine_sched_cores.size() == 0)
        {
            std::cout << "No schedulable solution for: " << runnable->getName() << std::endl;
            auto pbl = std::find(PBL.begin(), PBL.end(), runnable);
            auto bl = std::find(BL.begin(), BL.end(), runnable);
            
            if (pbl != PBL.end())
            {
                std::cout << "Unable to find a schedulable solution" << std::endl;
                exit(-1);
            }
            
            // "runnable" was in the black list, now put it in the post black
            //  list
            else if (bl != BL.end())
            {
                std::cout << "Try to assign task " << runnable->getName() << " in a non affine Core" << std::endl;
                PBL.push_back(runnable);
            }
            
            // "runnable" was not in the black list, now put it in the black list
            else
            {
                //std::cout << "Try with another solution for task " << runnable->getName() << std::endl;
                BL.push_back(runnable);
            }
            
            // Deallocate the affine set
            auto affine_set = runnable->getPredecessors();
            
            std::cout << "De-allocate the affine set" << std::endl;
            for (auto affine_elem : affine_set)
                std::cout << " " << affine_elem.first->getName() << std::endl;
            
            for (auto affine_elem : affine_set)
                for (auto c : cores)
                    c->removeFunction(affine_elem.first);
            
            // Deallocate the current runnable
            for (auto c : affine_cores)
                c->removeFunction(runnable);
            
            // The affine set must be re-allocated
            for (auto t : affine_set)
                NAR.push_back(t.first);
        }
        for (auto core : cores)
            core->checkSchedulability();
    }
}

void Processor::print(std::ostream &stream)
{
    int i = 0;
    for (auto core: cores)
    {
        stream << "Core: " << i++ << std::endl;
        core->printTs(stream);
        stream << std::endl;
    }
    
//    i = 0;
//    for (auto core: cores)
//    {
//        stream << "Core: " << i++ << std::endl;
//        core->printRbfs(stream);
//        stream << std::endl;
//    }
}


long Processor::getCoreIdx(Function *f)
{
    int c = 0;
    for (auto core : cores)
    {
        for (auto task : core->getTs())
            if (task->isMy(f))
                return c;
        c++;
    }
    std::cout << __FILE__ << " " << __FUNCTION__
              << ": Function not found" << std::endl;
    exit(-1);
}

TaskSet* Processor::getCore(Function *f)
{
    for (auto core : cores)
        for (auto task : core->getTs())
            if (task->isMy(f))
                return core;
    return nullptr;
}

void Processor::printInternalStats(std::vector<TaskSet* > cores)
{
    long core_idx = 0;
    for (auto core : cores)
    {
        auto slacks = core->getSlack();
        cout << "Slack for core: " << core_idx++ << endl;
        for (auto slack : slacks) {
            cout << slack << " ";
        }
        cout << endl;
    }
}

void Processor::computeRT()
{
    total_size = 0;
    rt1_size = 0;
    rt2_size = 0;
    
    for (auto c : cores)
    {
        c->computeRT();
        total_size += c->getRT();
        rt1_size += c->getRT1();
        rt2_size += c->getRT2();
    }
    
    for (auto fun: runnables)
    {
        for (auto succ : fun->getSuccessors())
        {
            auto core_f = getCore(fun);
            auto core_s = getCore(succ.first);
            
            if (core_f != core_s)
            {
                if (fun->getPeriod() > succ.first->getPeriod())
                {
                    rt1_size += succ.second;
                    total_size += succ.second;
                }
                if (fun->getPeriod() < succ.first->getPeriod())
                {
                    rt2_size += succ.second;
                    total_size += succ.second;
                }
            }
        }
    }
}

long Processor::getRT1()
{
    return rt1_size;
}

long Processor::getRT2()
{
    return rt2_size;
}

long Processor::getRT()
{
    return total_size;
}







