
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
#include <map>

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

std::vector<Function*> Processor::getAllocatedRunnables()
{
    std::vector<Function*> runnables;
    for (auto core : cores)
        for (auto task : core->getTs())
            for (auto function : task->getFunctions())
                runnables.push_back(function);
    return runnables;
}

std::map<Function*, float> Processor::getResponseTimes()
{
    std::map<Function*, float> resp_t;
    auto runnables = getAllocatedRunnables();
    for (auto r : runnables)
    {
        auto core = getCore(r);
        auto r_t = core->getResponseTime(r);
        if (r_t > 0)
            resp_t[r] = r_t;
        else
        {
            std::cout << __FUNCTION__ << " - Response time == -1" << std::endl;
            exit(-1);
        }
    }
    return resp_t;
}

bool Processor::checkSchedulability(std::map<Function*, float>& resp_t)
{
    for (auto f : resp_t)
        if (f.second == -1)
            return false;
    return true;
}

std::map<Task*, long> Processor::getOffsets()
{
    std::map<Task*, long> offsets;
    for (auto core : cores)
        for (auto task : core->getTs())
            offsets[task] = task->getOffset();
    return offsets;
}

std::map<Task*, float> Processor::getNewOffsets()
{
    std::map<Task*, float> offsets_t;
    std::map<Function*, float> respt_f;
    
    auto allocated_f = getAllocatedRunnables();
    
    for (auto f : allocated_f)
    {
        auto c_t = getCore(f)->getCompletionTime(f);
        if (c_t > 0)
            respt_f[f] = c_t;
        else
        {
            std::cout << __FUNCTION__ << " - Completion time == -1" << std::endl;
            exit(-1);
        }
    }
    
    for (auto c : cores)
    {
        for (auto t : c->getTs())
        {
            float offset = 0;
            for (auto f : t->getFunctions())
            {
                for (auto p : f->getPredecessors())
                    if (respt_f[p.first] > offset) offset = respt_f[p.first];
            }
            offsets_t[t] = offset;
        }
    }
    
    return offsets_t;
}

std::vector<TaskSet*> Processor::getAllocCores()
{
    std::vector<TaskSet*> alloc_cores;
    for (auto core : cores)
        if (core->getTs().size() != 0)
            alloc_cores.push_back(core);
    return alloc_cores;
}


std::map<Task*, float> Processor::setNewOffsets()
{
    auto offsets = getNewOffsets();
    for (auto t : offsets)
        t.first->setOffset(t.second);
    return offsets;
}

void Processor::setOffsets(std::map<Task*, float> offsets)
{
    for (auto t : offsets)
        t.first->setOffset(t.second);
}

bool Processor::checkSchedulability()
{
    auto alloc_cores = getAllocCores();
    for (auto core : alloc_cores)
        if (core->checkSchedulability() == false)
            return false;
    return true;
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
        
        std::map<Function*, float> resp_times;
        
        // Contains the minimum slacks among all the cores for the assignment
        // of a runnable in a given affine core
        std::map<TaskSet*, float> slacks;
        std::map<TaskSet*, std::map<Task*, float> > offsets_c;
        
        for (auto core : affine_cores)
        {
            std::cout << "Trying on core: " <<
                (std::find(cores.begin(), cores.end(), core) - cores.begin()) <<
                std::endl;
            
            // Take the response times before runnable allocation
            auto orig_resp_times = getResponseTimes();
            auto orig_offsets = getOffsets();
            
            // Allocate runnable
            core->addFunction(runnable);
            
            // Take response times
            auto resp_times = orig_resp_times;
            
            // Take the new response times
            auto tmp_resp_t = getResponseTimes();
            
            // Add the response time of the new runnable to "resp_times"
            resp_times[runnable] = tmp_resp_t[runnable];

            // The offsets
            std::map<Task*, float> offsets;
            
            // Resolve fixed point problem
            int limit = 0;
            while (tmp_resp_t != resp_times && ++limit < 100)
            {
                //std::cout << "Trying to converge" << std::endl;
                // old response times = current response times
                resp_times = tmp_resp_t;
                
                // Set new offsets
                offsets = setNewOffsets();
                
                // compute new response times
                tmp_resp_t = getResponseTimes();
                
            }
            
            if (tmp_resp_t == resp_times)
            {
                // Check if the solution is schedulable
                auto is_sched = checkSchedulability();
                
                //            std::cout << "Response times:" << std::endl;
                //            for (auto r : tmp_resp_t)
                //                std::cout << r.second << " ";
                //            std::cout << std::endl;
                
                // If the solution is schedulable, store the results (slack)
                if (is_sched)
                {
                    std::vector<float> tmp_slacks;
                    auto alloc_cores = getAllocCores();
                    for (auto c : alloc_cores)
                    {
                        auto slack = c->getMinSlack();
                        if (slack >= 0)
                            tmp_slacks.push_back(slack);
                        else
                        {
                            std::cout << "ERROR: negative slack with schedulable solution" << std::endl;
                            std::cout << runnable->getName() << ", slack: " << slack << std::endl;
                            exit(-1);
                        }
                    }
                    
                    // Take the minimum slack among all the cores for the allocation
                    // of "runnable" in this affine core
                    slacks[core] = *std::min_element(tmp_slacks.begin(), tmp_slacks.end());
                    offsets_c[core] = offsets;
                }
                std::cout << std::endl;
                
                // Remove the assignment and restore the taskset
                core->removeFunction(runnable);
            }
            else
            {
                // Remove the assignment and restore the taskset
                core->removeFunction(runnable);
            }
        }
        
        std::cout << "Affine schedulable solutions: " << slacks.size() << std::endl;
        
        // If I found schedulable solutions
        if (slacks.size() != 0)
        {
            // Find the allocation that maximize the minimum slack
            TaskSet* best_core = slacks.begin()->first;
            for (auto i : slacks)
                if(i.second > slacks[best_core]) best_core = i.first;
            
            // Restore the best solution
            best_core->addFunction(runnable);
            setOffsets(offsets_c[best_core]);

            std::cout << "Runnable " << runnable->getName() << " allocated on core " << std::distance(cores.begin(), std::find(cores.begin(), cores.end(), best_core)) << " with offset: " << best_core->getTask(runnable)->getOffset() << std::endl;

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
        
        else if (slacks.size() == 0)
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

    } // while( NAR.size() != 0 )
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







