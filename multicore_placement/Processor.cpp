
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
#include <cmath>

#define DEBUG_ICA

#define LIMIT 10

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
    
    for (auto core : cores)
    {
        auto tasks = core->getTs();
        for (auto task : tasks)
        {
            auto functions = task->getFunctions();
            for (auto function : functions)
            {
                alloc_r.push_back(function);
            }
        }
    }
    
//    for (auto r : runnables)
//    {
//        auto f = std::find(NAR.begin(), NAR.end(), r);
//        if (f == NAR.end())
//            alloc_r.push_back(r);
//    }
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

std::map<Function*, float> Processor::getCompletionTimes()
{
    std::map<Function*, float> compl_t;
    auto runnables = getAllocatedRunnables();
    for (auto r : runnables)
    {
        auto core = getCore(r);
        auto r_t = core->getCompletionTime(r);
        compl_t[r] = r_t;
    }
    return compl_t;
}

bool Processor::checkSchedulability(std::map<Function*, float>& resp_t)
{
    for (auto f : resp_t)
        if (f.second == -1)
            return false;
    return true;
}

std::map<Task*, float> Processor::getOffsets()
{
    std::map<Task*, float> offsets;
    for (auto core : cores)
        for (auto task : core->getTs())
            offsets[task] = task->getOffset();
    return offsets;
}

std::map<Task*, float> Processor::getNewOffsets()
{
    std::map<Task*, float> offsets_t;
    std::map<Function*, float> complt_f;
    
#ifdef DEBUG_ICA
    std::cout << "Actual offsets => ";
    for (auto core : cores)
    {
        auto tasks = core->getTs();
        
        for (auto task : tasks)
            std::cout << " " << task->getName() << ": " << task->getOffset();
        
    }
    std::cout << std::endl;
#endif
    
    auto allocated_f = getAllocatedRunnables();
    
    for (auto function : allocated_f)
    {
        auto c_t = getCore(function)->getCompletionTime(function);
        complt_f[function] = c_t;
    }
    
#ifdef DEBUG_ICA
    std::cout << "Actual completion times => ";
    for (auto function : allocated_f)
        std::cout << " " << function->getName() << ": " << complt_f[function];
    std::cout << std::endl;
#endif
    int i = 0;
    for (auto core : cores)
    {
#ifdef DEBUG_ICA
        std::cout << "Core: " << i++ << std::endl;
#endif
        auto tasks = core->getTs();
        for (auto task : tasks)
        {
            float global_offset = 0.0;
            float start_time = 0.0;
            
            auto functions = task->getFunctions();
            
            for (auto function : functions)
            {
                auto preds = function->getPredecessors();

                float tmp_offset = 0.0;
                
                // Determine the offset for function
                for (auto p : preds)
                {
                    if (complt_f[p.first] > tmp_offset &&
                        getCore(p.first) != getCore(function))
                    {
                        tmp_offset = complt_f[p.first];
                    }
                }
                
                // Determine the start time of the function
                start_time = core->getStartTime(function);
#ifdef DEBUG_ICA
                std::cout << "Function: " << function->getName() << " start at: "
                << start_time << std::endl;
                std::cout << "Global offset: " << tmp_offset << std::endl;
#endif
                // If the start time of the function is < of the offset needed
                // to the function itself, consider the offset
                if (start_time < tmp_offset)
                    // If the function offset is > of the global offset computed
                    // untill now, than consider global_offset = tmp_offset
                    if (tmp_offset > global_offset)
                        global_offset = tmp_offset;
                
            }

            if (global_offset > task->getOffset())
                offsets_t[task] = global_offset;
            else
                offsets_t[task] = task->getOffset();
        }
    }
#ifdef DEBUG_ICA
    std::cout << "New computed offsets: ";
    for (auto task: offsets_t)
        std::cout << task.first->getName() << ": " << task.second << " ";
    std::cout << std::endl;
#endif
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
    // Get new offsets and returns they by tasks
    std::map<Task*, float> offsets = getNewOffsets();
    
    for (auto task : offsets)
        task.first->setOffset(task.second);
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

void Processor::adjustPriorities()
{
    for (auto core : cores)
        core->adjustPriorities();
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

std::pair<TaskSet*, float> Processor::computeOffset(Function *f, TaskSet *c)
{
    float offset = 0.0;
    TaskSet* core = nullptr;
    Function* function = nullptr;
    auto preds = f->getPredecessors();
    
    for (auto pred : preds)
    {
        Function* p = pred.first;
        auto ct = getCore(p)->getCompletionTime(p);
        if (ct > offset && getCore(p) != c)
        {
            offset = ct;
            core = getCore(p);
            function = p;
        }
    }
    return make_pair(core, offset);
}

void Processor::discoverLoop(Function *f, TaskSet* c, long i,
                             std::vector<long> &p, std::vector<std::string> &l)
{
    auto predecessors0 = f->getPredecessors();
    std::vector<long> priorities;
    
//    std::cout << "Funcion: " << f->getName() << ": ";
//    for (auto pred0 : predecessors0)
//        std::cout << pred0.first->getName() << " ";
//    std::cout << std::endl;
    
    for (auto pred0 : predecessors0)
    {
        auto p0 = pred0.first;
        auto my_core = getCore(p0);
        
        // If my_core == core and the index i == -1, I found a loop
        // Store the priority of the task in which p0 is allocated
        if (my_core == c && i == -1)
        {
            //std::cout << "Closed loop: " << p0->getName() << std::endl;
            p.push_back(my_core->getPriority(p0));
            l.push_back(p0->getName());
            continue;
        }
        
        // If my_core != core, there is a possible loop.
        // I keep track of this changes of core (for a predecessor) by storing
        // -1 in i
        
        else if (my_core != c && my_core != nullptr)
        {
            //std::cout << "Possible loop: " << p0->getName() << std::endl;
            discoverLoop(p0, c, -1, p, l);
            //l.push_back(p0->getName());
        }
        else
        {
            //std::cout << "Discovering loop: " << p0->getName() << std::endl;
            discoverLoop(p0, c, 0, p, l);
        }
        
    }
}

void Processor::printPercentage()
{
    std::cout << "Total: " << runnables.size() << ", left: "
    << NAR.size() << std::endl;
}

float Processor::getMaxDiff(std::map<Function*, float> A,
                            std::map<Function*, float> B)
{
    float max = 0.0;
    for (auto f : A)
        if (max < std::abs(A[f.first] - B[f.first]))
            max = std::abs(A[f.first] - B[f.first]);
    return max;
}

bool Processor::interCoreAllocation(float Ub)
{
    std::vector<Function*> BL;
    std::vector<Function*> PBL;
    
    ofstream errors;
    errors.open("errors.txt", ios::out | ios::app);
    
    while (NAR.size() != 0)
    {
#ifdef DEBUG
        printPercentage();
#endif
        
        // Take the enabled tasks and order them by density
        auto en_runnables = getEnabledRunnables();
        std::sort(en_runnables.begin(), en_runnables.end(), sort_Fdensity);
        auto runnable = en_runnables.front();
        
        // Get affine cores
        std::vector<TaskSet*> affine_cores;
        if (std::find(PBL.begin(), PBL.end(), runnable) == PBL.end())
            affine_cores = getAffineCores(runnable, Ub);
        
        else
            affine_cores = cores;

#ifdef DEBUG_ICA
        std::cout << std::endl << "Trying to allocate: " << runnable->getName() << std::endl;
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
        std::map<TaskSet*, std::pair<long, float>> priorities_c;
        
        for (auto core : affine_cores)
        {

            // Take the completion times before runnable allocation
            auto compl_times = getCompletionTimes();
            
            // The offsets before the assignment
            // They are used to restore the offsets after I tried to allocate
            // a runnable
            auto orig_offsets = getOffsets();

#ifdef DEBUG_ICA
            std::cout << std::endl << "TRYING ON CORE: "
            << std::distance(cores.begin(),
                             std::find(cores.begin(), cores.end(), core))
            << std::endl << std::endl;
            
            std::cout << "Offsets before allocation => ";
            for (auto t : orig_offsets)
                std::cout<< t.first->getName() << ": " << t.second << " ";
            std::cout << std::endl;
#endif
            
            // Allocate runnable
            // If the worst offset is with respect to core, assign runnable
            // without offset, otherwise assign with offset
            
            std::pair<TaskSet*, float> core_offset = computeOffset(runnable, core);
            float max_off = core_offset.second;
            
            std::pair<float, long> hp_allocation = core->testRunnable(runnable);
            float hp_start_time = hp_allocation.first;
            
            // Discover loops
            std::vector<long> priorities;
            std::vector<std::string> loops;
            loops.push_back(runnable->getName());
            
            discoverLoop(runnable, core, 0, priorities, loops);
            long loop_priority = 0;
            
            if (priorities.size() != 0)
            {

                
                loop_priority = *std::max_element(priorities.begin(),
                                                  priorities.end());
#ifdef DEBUG_ICA
                std::cout << "LOOPS identified:" << std::endl << "\t";
                for (auto function : loops)
                    std::cout << function << " ";
                std::cout << std::endl;
#endif
            }
            
            // In case the hypotetical start time of the runnable in core is
            // higher or equal of the maximum offset, then allocate without
            // offset
            if (hp_start_time >= max_off)
            {

                if (loop_priority == 0)
                    core->addFunction(runnable);
                else
                    core->addFunction(runnable, loop_priority);
#ifdef DEBUG_ICA
                std::cout << "Allocation without offset" << std::endl;
                core->printTsExt(std::cout);
#endif
            }
            
            // In case the hypotetical start time of the runnable in core is
            // lower of the maximum offset, then allocate with offset
            else
            {
                if (loop_priority == 0)
                    core->addFunction(runnable, max_off);
                else
                    core->addFunction(runnable, max_off, loop_priority);
#ifdef DEBUG_ICA
                std::cout << "Allocation with offset: " << max_off << std::endl;
                core->printTsExt(std::cout);
#endif
            }
            
            long P = core->getTask(runnable)->getPriority();
            float O = core->getTask(runnable)->getOffset();
            
            priorities_c[core] = std::make_pair(P, O);
            
            // Take the new completion times
            auto new_compl_t = getCompletionTimes();
            
            // Add the response time of the new runnable to "resp_times"
            //tmp_compl_t[runnable] = core->getCompletionTime(runnable);
            compl_times[runnable] = new_compl_t[runnable];
            
            // The offsets
            std::map<Task*, float> offsets;
            
            // Resolve fixed point problem
            int limit = 0;
            auto diff = 2;
            
#ifdef DEBUG_ICA
            std::cout << "Solving FP..." << std::endl;
#endif
            
            while (diff >= 1 && ++limit < LIMIT)
            {
                // Save the old completion times
                for (auto t : new_compl_t)
                    compl_times[t.first] = t.second;
                
                // Set new offsets
                offsets = setNewOffsets();

                // compute new response times
                new_compl_t = getCompletionTimes();
                
                // compute diff
                diff = getMaxDiff(compl_times, new_compl_t);
                
#ifdef DEBUG_ICA
                
                std::cout << "Completion times before => ";
                for (auto c : compl_times)
                    std::cout << " " << c.first->getName() << ": " << c.second;
                std::cout << std::endl;
                std::cout << "Completion times after => ";
                for (auto c : new_compl_t)
                    std::cout << " " << c.first->getName() << ": " << c.second;
                std::cout << std::endl;
                
                std::cout << "Max diff: " << diff << std::endl;
#endif
            }
             
#ifdef DEBUG_ICA
            std::cout << "...FP SOLVED!" << std::endl;
            std::cout << "Offsets after allocation => ";
            for (auto t : offsets)
                std::cout << " " << t.first->getName() << ": " << t.second;
            std::cout << std::endl;
#endif
            
            // If the solution converge and is schedulable
            bool is_sched = checkSchedulability();
            
            if (diff <= 1 && is_sched == true)
            {
#ifdef DEBUG_ICA
                std::cout << "Schedulable on this core" << std::endl;
#endif
                // If the solution is schedulable, store the results (slack)

                std::vector<float> tmp_slacks;
                auto alloc_cores = getAllocCores();
                for (auto c : alloc_cores)
                {
                    float slack = c->getMinSlack();
                    std::cout << "====> Slack: " << slack << std::endl;
                    tmp_slacks.push_back(slack);
                }
                
                // Take the minimum slack among all the cores for the allocation
                // of "runnable" in this affine core
                slacks[core] = *std::min_element(tmp_slacks.begin(),
                                                 tmp_slacks.end());
                
                std::map<Function*, float> tmp;
                for (auto offset : offsets)
                {
                    for (auto function : offset.first->getFunctions())
                    {
                        tmp[function] = offset.second;
                    }
                }
                offsets_c[core] = tmp;
            }
            else
            {
#ifdef DEBUG_ICA
                std::cout << "diff: " << diff << ", is_sched: " << is_sched << std::endl;
#endif
            }
            
            // Remove the assignment and restore the offsets
            for (auto core : cores)
                for (auto task : core->getTs())
                    task->setOffset(orig_offsets[task]);

            core->removeFunction(runnable);
            
            adjustPriorities();
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
            
            //std::cout << "Slacks: " << slacks.size() << " => ";
            for (auto slack : slacks)
            {
                TaskSet* core = slack.first;
                auto core_idx = std::distance(cores.begin(),
                                              std::find(cores.begin(),
                                                        cores.end(), core));
                std::cout << core_idx << ": " << slack.second << " ";
            }
            std::cout << std::endl;
  
            for (auto i : slacks)
                if(i.second > slacks[best_core])
                    best_core = i.first;
            
            std::vector<TaskSet* > best_cores_list;
            for (auto i : slacks)
                if (slacks[best_core] == i.second)
                    best_cores_list.push_back(i.first);
            
            std::vector<float> best_cores_load;
            for (auto core : best_cores_list)
                best_cores_load.push_back(core->getLoad());
            
            float best_load = *std::min_element(best_cores_load.begin(), best_cores_load.end());
            for (auto core : best_cores_list)
            {
                if (core->getLoad() == best_load)
                {
                    best_core = core;
                    break;
                }
            }
            
            // Restore the best solution
            // - allocate the runnable on the best core
            // - restore the tasks offset for each task in all the cores
            best_core->addFunction(runnable, priorities_c[best_core].first,
                                   priorities_c[best_core].second);
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
            
#ifdef DEBUG_ICA
            std::cout << "Allocated runnables: ";
            auto allocated = getAllocatedRunnables();
            for (auto function : allocated)
                std::cout << function->getName() << " ";
            std::cout << std::endl;
#endif

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
                std::cout << "Unable to find a schedulable solution"
                << std::endl;
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
#ifdef DEBUG_ICA
                std::cout << "Try with another solution for task "
                << runnable->getName() << std::endl;
#endif
                BL.push_back(runnable);
            }

            // Deallocate the affine set
            auto affine_set = runnable->getPredecessors();

            // I do not have to de allocate the runnable, since it is always
            // removed andded only if no schedulable solutions have been found
            // and if I am here, there are no schedulable solutions
            
            // However, I have to de-allocate the affine set
            for (auto affine_elem : affine_set)
            {
                auto core = getCore(affine_elem.first);
                core->removeFunction(affine_elem.first);
            }
            
#ifdef DEBUG_ICA
            std::cout << "De-allocate the affine set: ";
            for (auto affine_elem : affine_set)
                std::cout << " " << affine_elem.first->getName();
            std::cout << std::endl << "Affine set de-allocated" << std::endl;
#endif
            
            // The affine set must be re-allocated
            for (auto f : affine_set)
                NAR.push_back(f.first);
            
            // Offsets and schedulability analysis must be computed again
            std::map<Function*, float> new_compl_t = getCompletionTimes();
            std::map<Function*, float> compl_times;
            
            long limit = 0;
            auto diff = 2;
            auto offsets = getOffsets();
            
            clearOffsets();
            
            while (diff >= 1 && ++limit < LIMIT)
            {
                // old response times = current response times
                compl_times = new_compl_t;
                
                // Set new offsets
                offsets = setNewOffsets();
                
                // compute new response times
                new_compl_t = getCompletionTimes();
                
                diff = getMaxDiff(compl_times, new_compl_t);
                
#ifdef DEBUG_ICA
                
                std::cout << "Completion times before => ";
                for (auto c : compl_times)
                    std::cout << " " << c.first->getName() << ": " << c.second;
                std::cout << std::endl;
                std::cout << "Completion times after => ";
                for (auto c : new_compl_t)
                    std::cout << " " << c.first->getName() << ": " << c.second;
                std::cout << std::endl;
                
                std::cout << "Max diff: " << diff << std::endl;
#endif
            }
            
#ifdef DEBUG_ICA
            std::cout << "...FP SOLVED!" << std::endl;
            std::cout << "Offsets after allocation => ";
            for (auto t : offsets)
                std::cout << " " << t.first->getName() << ": " << t.second;
            std::cout << std::endl;
#endif
            
            bool is_sched = checkSchedulability();
            
            if (diff > 1 || is_sched == false)
            {
                errors << "ERROR" << std::endl;
#ifdef DEBUG_ICA
                std::cout << "ERROR: unable to find a fixed point solution ";
                std::cout << " - diff: " << diff << ", is_sched: " << is_sched
                << std::endl;
#endif
                return false;
            }
            
        }
#ifdef DEBUG_ICA
        std::cout << std::endl << "ALLOCATION: " <<std::endl;
        print(std::cout);
        
        slacks.clear();
        offsets_c.clear();
        priorities_c.clear();
        
        std::cout << std::endl << std::endl << " ======== NEXT ========" << std::endl << std::endl;
#endif
    } // while( NAR.size() != 0 )
    
    //computeRT();
    //optimizeRT2();
    
    errors.close();
    
    
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
    std::map<TaskSet*, std::pair<Function*, Function*>> worst_links;
    std::map<TaskSet*, long> worst_links_size;
    
    // This for loop MUST be executed only once and only here.
    // Each time you call core->getLink(), the previous (bigger) link is
    // discarded.
    for (auto core : cores)
    {
        auto link = core->getLink();
        if (link.first != nullptr && link.second != nullptr)
        {
            worst_links[core] = link;
            worst_links_size[core] = link.first->getLinkSize(link.second);
        }
    }
    
    TaskSet *CORE = (*worst_links.begin()).first;
    
    while (worst_links.size() != 0)
    {
        // Determine the taskset with the heaviest link
        // CORE is the TaskSet to optimize
        CORE = (*worst_links_size.begin()).first;
        auto size = (*worst_links_size.begin()).second;
        
        for (auto l : worst_links_size)
        {
            if (l.second > size)
            {
                CORE = l.first;
                size = l.second;
            }
        }
        
        computeRT();
        
        auto memory_before = total_size;
        //auto cores_backup = cores;
        auto orig_offsets = getOffsets();
        
#ifdef DEBUG_ICA
        std::cout << "optimizing: " << worst_links[CORE].first->getName()
        << " -> " << worst_links[CORE].second->getName() << std::endl;
#endif
        // One step optimization
        auto LINK = worst_links.at(CORE);
        Function* source_f = LINK.first;
        Function* destin_f = LINK.second;
        
        CORE->optimize(source_f, destin_f);
        
        // Re-compute the RT blocks
        computeRT();
        
        // If the optimization lead to a higer memory cost, I have to discard this
        // optimization and restore the previous taskset
        if (memory_before < total_size)
        {
            //std::cout << "Before: " << memory_before << ", After: " << total_size << std::endl;
            
            CORE->undoOptimize();
            
            computeRT();
            
            if (memory_before < total_size)
            {
                
                std::cout << "ERROR: restored bad solution but still bad" << std::endl;
                std::cout << "Before: " << memory_before << ", After: " << total_size << std::endl;

                exit(-1);
            }
        }
        else
        {
            std::map<Function*, float> tmp_comple_t = getCompletionTimes();
            std::map<Function*, float> comple_times;
            long limit = 0;
            
            // Try to find a convergent solution
            clearOffsets();
            while (tmp_comple_t != comple_times && ++limit < 100)
            {
                // old response times = current response times
                comple_times = tmp_comple_t;
                
                // Set new offsets
                setNewOffsets();
                
                // compute new response times
                tmp_comple_t = getCompletionTimes();
            }
            
            bool is_sched = checkSchedulability();
            
            if (tmp_comple_t != comple_times || is_sched == false)
            {
                CORE->undoOptimize();
                
                // Restore offsets
                for (auto core : cores)
                    for (auto task : core->getTs())
                        task->setOffset(orig_offsets[task]);
            }
            
            // Confirm the optimization only if the solution is schedulable
            else if ( tmp_comple_t == comple_times && is_sched == true)
                CORE->confirmOptimization();
        }
        
        // Take the next link to optimize for CORE
        auto new_link = CORE->getLink();
        source_f = new_link.first;
        destin_f = new_link.second;
        
        if (source_f != nullptr && destin_f != nullptr)
        {
            worst_links[CORE] = new_link;
            worst_links_size[CORE] = source_f->getLinkSize(destin_f);
        }
        else
        {
            worst_links.erase(CORE);
            worst_links_size.erase(CORE);
        }
        
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







