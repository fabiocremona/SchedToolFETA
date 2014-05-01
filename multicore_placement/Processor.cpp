
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

//#define DEBUG_ICA

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

Processor::Processor(std::vector<Function*> runn, int n_cores)
{
    for (auto i = 0 ; i < n_cores; i++)
        cores.push_back(new TaskSet());
    NAR = runn;
    runnables = runn;
    
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
    
    return affine;
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
    std::vector<Function*> alloc_r;
    for (auto r : runnables)
    {
        auto f = std::find(NAR.begin(), NAR.end(), r);
        if (f == NAR.end())
            alloc_r.push_back(r);
    }
    return alloc_r;
}

std::map<Function*, float> Processor::getResponseTimes()
{
    std::map<Function*, float> resp_t;
    auto runnables = getAllocatedRunnables();
    for (auto r : runnables)
    {
        auto core = getCore(r);
        auto r_t = core->getResponseTime(r);
        resp_t[r] = r_t;
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
        respt_f[f] = c_t;
    }
    
    for (auto c : cores)
    {
        for (auto t : c->getTs())
        {
            float offset = 0;
            for (auto f : t->getFunctions())
            {
                for (auto p : f->getPredecessors())
                    if (respt_f[p.first] > offset)
                        offset = respt_f[p.first];
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

void Processor::setOffsets(std::map<Function*, float> offsets)
{
    for (auto core : cores)
    {
        for (auto function : offsets)
        {
            if (core->isMy(function.first))
            {
                auto task = core->getTask(function.first);
                task->setOffset(function.second);
                
            }
        }
    }
}

void Processor::clearOffsets()
{
    for (auto core : cores)
        for (auto task : core->getTs())
            task->setOffset(0);
}

bool Processor::checkSchedulability()
{
    auto alloc_cores = getAllocCores();
    for (auto core : alloc_cores)
        if (core->checkSchedulability() == false)
        {
#ifdef DEBUG_P_R
            std::cout << "This core is not schedulable: "
            << std::distance(cores.begin(),
                             std::find(cores.begin(),
                                       cores.end(),
                                       core)) << std::endl;
#endif
            return false;
        }
    return true;
}

std::pair<TaskSet*, float> Processor::getOffset(Function *f)
{
    float offset = 0;
    TaskSet* core = nullptr;
    for (auto p : f->getPredecessors())
    {
        auto ct = getCore(p.first)->getCompletionTime(p.first);
        if (ct > offset)
        {
            offset = ct;
            core = getCore(p.first);
        }
    }
    return make_pair(core, offset);
}

void Processor::printPercentage()
{
    std::cout << "Total: " << runnables.size() << ", left: "
    << NAR.size() << std::endl;
}

bool Processor::interCoreAllocation(float Ub)
{
    std::cout << "Starting allocating..." << std::endl;
    std::vector<Function*> BL;
    std::vector<Function*> PBL;
    
    while (NAR.size() != 0)
    {
        printPercentage();
        
        // Take the enabled tasks and order them by density
        auto en_runnables = getEnabledRunnables();
        std::sort(en_runnables.begin(), en_runnables.end(), sort_Fdensity);
        auto runnable = en_runnables.front();
        
#ifdef DEBUG_ICA
        std::cout << "Trying to allocate: " << runnable->getName() << std::endl;
#endif
        
        // Get affine cores
        std::vector<TaskSet*> affine_cores;
        if (std::find(PBL.begin(), PBL.end(), runnable) == PBL.end())
            affine_cores = getAffineCores(runnable, Ub);
        else affine_cores = cores;
        
#ifdef DEBUG_ICA
        std::cout << "Affine cores " << affine_cores.size() << std::endl;
        for (auto c : affine_cores)
            std::cout << "\t" << std::distance(cores.begin(),
                                               std::find(cores.begin(),
                                                         cores.end(),
                                                         c)) << ", ";
        std::cout << std::endl;
#endif
        
        // Contains the minimum slacks among all the cores for the assignment
        // of a runnable in a given affine core
        std::map<TaskSet*, float> slacks;
        
        // Contains all the offsets for each task in all the cores for each
        // assignment of runnable in an affine core
        std::map<TaskSet*, std::map<Function*, float> > offsets_c;
        
        for (auto core : affine_cores)
        {

            // Take the response times before runnable allocation
            auto resp_times = getResponseTimes();
            
            // The offsets before the assignment
            auto orig_offsets = getOffsets();

#ifdef DEBUG_ICA
            std::cout << "Trying on core: "
            << std::distance(cores.begin(),
                             std::find(cores.begin(), cores.end(), core))
            << std::endl;
            
            std::cout << "Offsets before allocation:" << std::endl;
            for (auto t : orig_offsets)
                std::cout << "\t" << t.first->getName() << ": " << t.second
                << std::endl;
#endif
            
            // Allocate runnable
            // If the worst offset is with respect to core, assign runnable
            // without offset, otherwise assign with offset
            
            auto core_offset = getOffset(runnable);
            if (core_offset.first == core)
                core->addFunction(runnable);
            else
                core->addFunction(runnable, core_offset.second);
            
            // Take the new response times
            auto tmp_resp_t = getResponseTimes();
            
            // Add the response time of the new runnable to "resp_times"
            resp_times[runnable] = tmp_resp_t[runnable];

            // The offsets I use
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
            offsets = setNewOffsets();
            
#ifdef DEBUG_ICA
            std::cout << "Offsets after allocation:" << std::endl;
            for (auto t : offsets)
                std::cout << "\t" << t.first->getName() << ": " << t.second
                << std::endl;
#endif
            
            // If the solution converge and is schedulable
            if (tmp_resp_t == resp_times && checkSchedulability())
            {
#ifdef DEBUG_ICA
                std::cout << "Schedulable" << std::endl;
#endif
                // If the solution is schedulable, store the results (slack)

                std::vector<float> tmp_slacks;
                auto alloc_cores = getAllocCores();
                for (auto c : alloc_cores)
                {
                    auto slack = c->getMinSlack();
                    tmp_slacks.push_back(slack);
                }
                
                // Take the minimum slack among all the cores for the allocation
                // of "runnable" in this affine core
                slacks[core] = *std::min_element(tmp_slacks.begin(),
                                                 tmp_slacks.end());
                
                for (auto offset : offsets)
                {
                    for (auto function : offset.first->getFunctions())
                    {
                        std::map<Function*, float> tmp;
                        tmp[function] = offset.second;
                        offsets_c[core] = tmp;
                    }
                }

            }
            
            // Remove the assignment and restore the offsets
            for (auto core : cores)
                for (auto task : core->getTs())
                    task->setOffset(orig_offsets[task]);

            core->removeFunction(runnable);
            
        }
#ifdef DEBUG_ICA
        std::cout << "Affine schedulable solutions: " << slacks.size()
        << std::endl;
#endif
        // If I found schedulable solutions
        if (slacks.size() != 0)
        {
            // Find the allocation that maximize the minimum slack
            TaskSet* best_core = slacks.begin()->first;
            for (auto i : slacks)
                if(i.second > slacks[best_core]) best_core = i.first;
            
            // Restore the best solution
            // - allocate the runnable on the best core
            // - restore the tasks offset for each task in all the cores
            best_core->addFunction(runnable);
            setOffsets(offsets_c[best_core]);

#ifdef DEBUG_ICA
            std::cout << "Runnable " << runnable->getName()
            << " allocated on core " << std::distance(cores.begin(),
                                                      std::find(cores.begin(),
                                                                cores.end(),
                                                                best_core))
            << " with offset: " << best_core->getTask(runnable)->getOffset()
            << " and response time: " << best_core->getResponseTime(runnable)
            << std::endl;
#endif
            
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
#ifdef DEBUG_ICA
            std::cout << "No schedulable solution for: " << runnable->getName()
            << std::endl;
#endif
            auto pbl = std::find(PBL.begin(), PBL.end(), runnable);
            auto bl = std::find(BL.begin(), BL.end(), runnable);

            if (pbl != PBL.end())
            {
                //std::cout << "Unable to find a schedulable solution"
                //<< std::endl;
                return false;
            }

            // runnable was in the black list, now put it in the post black
            //  list
            else if (bl != BL.end())
            {
#ifdef DEBUG_ICA
                std::cout << "Try to assign task " << runnable->getName()
                << " in a non affine Core" << std::endl;
#endif
                PBL.push_back(runnable);
            }

            // runnable was not in the black list, now put it in the black list
            else
            {
                //std::cout << "Try with another solution for task "
                //<< runnable->getName() << std::endl;
                BL.push_back(runnable);
            }

            // Deallocate the affine set
            auto affine_set = runnable->getPredecessors();
#ifdef DEBUG_ICA
            std::cout << "De-allocate the affine set" << std::endl;
            for (auto affine_elem : affine_set)
                std::cout << " " << affine_elem.first->getName() << std::endl;
#endif

            // I do not have to de allocate the runnable, since it is always
            // removed andded only if no schedulable solutions have been found
            // and if I am here, there are no schedulable solutions
            
            // However, I have to de-allocate the affine set
            for (auto affine_elem : affine_set)
            {
                auto core = getCore(affine_elem.first);
                core->removeFunction(affine_elem.first);
            }
            
            // The affine set must be re-allocated
            for (auto f : affine_set)
                NAR.push_back(f.first);
            
            // Offsets and schedulability analysis must be computed again
            
            std::map<Function*, float> tmp_resp_t = getResponseTimes();
            std::map<Function*, float> resp_times;
            long limit = 0;
            
            //clearOffsets();
            
            while (tmp_resp_t != resp_times && ++limit < 100)
            {
                // old response times = current response times
                resp_times = tmp_resp_t;
                
                // Set new offsets
                setNewOffsets();
                
                // compute new response times
                tmp_resp_t = getResponseTimes();
            }
            
            if (tmp_resp_t != resp_times)
            {
                //std::cout << "ERROR: I removed the affine set and I am not able"
                //<< " to find a schedulable solution anymore" << std::endl;
                return false;
            }
            
        }

    } // while( NAR.size() != 0 )
    return true;
}
        
void Processor::print(std::ostream &stream)
{
    int i = 0;
    for (auto core: cores)
    {
        stream << "Core: " << i++ << std::endl;
        core->printTsExt(stream);
        stream << std::endl;
    }
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

void Processor::optimizeRT2()
{
    // Allocate runnable
    // If the worst offset is with respect to core, assign runnable
    // without offset, otherwise assign with offset
    
    // Take the new response times
    auto resp_time = getResponseTimes();
    
    // 
    
    
//    // The offsets I use
//    std::map<Task*, float> offsets;
//    
//    // Resolve fixed point problem
//    int limit = 0;
//    while (tmp_resp_t != resp_times && ++limit < 100)
//    {
//        //std::cout << "Trying to converge" << std::endl;
//        // old response times = current response times
//        resp_times = tmp_resp_t;
//        
//        // Set new offsets
//        offsets = setNewOffsets();
//        
//        // compute new response times
//        tmp_resp_t = getResponseTimes();
//    }
//    offsets = setNewOffsets();
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







