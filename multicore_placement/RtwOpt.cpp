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

//  File: RtwOpt.cpp

#include "RtwOpt.h"
#include <algorithm>

RtwOpt::RtwOpt(TaskSet& ts)
{
    original_ts = ts;
    task_set = ts;
}

RtwOpt::~RtwOpt()
{
    
}

bool link_sort(std::pair<Function*, long>* a, std::pair<Function*, long>* b)
{
    return ( a->second > b->second);
}

long RtwOpt::computeRTB2()
{
    long buffer_size = 0;
    rt2s.clear();
    
    for (auto i = task_set.getTs().begin(); i != task_set.getTs().end(); i++)
    {
        for (auto j = (*i)->getFunctions().begin(); j != (*i)->getFunctions().end(); j++)
        {
            auto succs = (*j)->getSuccessors();
            for (auto l = succs.begin(); l != succs.end(); l++)
            {
                
                if ((*i)->getPriority() < task_set.getPriority((*l).first))
                {
                    if ((*l).first->getPeriod() % (*j)->getPeriod())
                        buffer_size += (2 * (*l).second);                        
                }
                
                if ((*i)->getPriority() > task_set.getPriority((*l).first))
                {
                    std::pair<Function*, Function*> link;
                    link = std::make_pair(*j, (*l).first);
                    buffer_size += (2 * (*l).second);
                    rt2s[link] = (*l).second;
                }
            }
        }
    }
    return buffer_size;
}

void RtwOpt::getSyncSet(std::vector<Function*>*sync_set, Function *f)
{
    for (auto i = f->getPredecessors().begin();
                        i != f->getPredecessors().end(); i++)
    {
        auto check = std::find(sync_set->begin(), sync_set->end(), (*i).first);
        if (f->getPeriod() == (*i).first->getPeriod() &&
                            check == sync_set->end())
        {
            sync_set->push_back((*i).first);
            getSyncSet(sync_set, (*i).first);
        }
    }
    
    for (auto i = f->getSuccessors().begin();
                        i != f->getSuccessors().end(); i++)
    {
        auto check = std::find(sync_set->begin(), sync_set->end(), (*i).first);
        if (f->getPeriod() == (*i).first->getPeriod() &&
                            check == sync_set->end())
        {
            sync_set->push_back((*i).first);
            getSyncSet(sync_set, (*i).first);
        }
    }
}

Task* RtwOpt::getOptTask(Function* f)
{
    auto dst_priority = task_set.getPriority(f);
    auto task = task_set.getTask(dst_priority);
    
    if (task == nullptr)
    {
        Task* new_task = new Task();
        new_task->changePriority(dst_priority);
        task_set.add(new_task);
        return new_task;
    }
    return task;
}

std::pair<Function*, Function*> RtwOpt::getLink()
{
    long width = -1;
    static long last_link_size = -1;
    std::pair<Function*, Function*> tmp_link = std::make_pair(nullptr, nullptr);
    static std::vector<std::pair<Function*, Function*>> last_links;
    
    if (last_link_size == -1)
    {
        for (auto i = rt2s.begin(); i != rt2s.end(); i++)
        {
            if ((*i).second > width)
            {
                width = (*i).second;
                tmp_link = (*i).first;
            }
        }
    }
    
    else {
        for (auto i = rt2s.begin(); i != rt2s.end(); i++)
        {
            auto l = std::find(last_links.begin(),
                               last_links.end(), (*i).first);
            if ((*i).second > width &&
                (*i).second <= last_link_size && l == last_links.end())
            {
                width = (*i).second;
                tmp_link = (*i).first;
            }
        }
    }
    
    last_links.push_back(tmp_link);
    last_link_size = width;
    return tmp_link;
}

TaskSet RtwOpt::optimize()
{
    std::cout << "*** Starting RT2 Optimizations ***" << std::endl;
    
    std::pair<Function*, Function*> null = std::make_pair(nullptr, nullptr);
    long buffer_size = computeRTB2();
    auto link = getLink();
    std::cout << " - Buffer Size due to RT2 before optimizations: "
              << buffer_size << std::endl;
    
    while ( link != null)
    {
        //std::cout << "link: " << link.first->getName() << " -> " << link.second->getName()
        //<< " " << rt2s[link] << std::endl;
        
        // Get the synchronous set of the source function
        std::vector<Function*> sync_set;
        sync_set.push_back(link.first);
        getSyncSet(&sync_set, link.first);
        
        // Get the task where the source function is mapped
        auto src_task = task_set.getTask(link.first);
        
        // Since I start from the RM assunption, each Synchronous Set is entirely
        // contained in the same task. This is not true if the RM assumption is
        // violated and could be source of errors.
        
        // Remove the Synchronous Set from the task.
        src_task->remove(&sync_set);
        if (src_task->getFunSize() == 0)
            task_set.remove(src_task);
        
        auto dst_task = getOptTask(link.second);
        
        dst_task->add(&sync_set);        
        
        task_set.adjustPriorities();
        
        bool is_sched = task_set.checkSchedulability();
        
        //std::cout << "Is sched: " << is_sched << std::endl;
        //std:: cout << "old: " << buffer_size << " new: " << computeRTB2() << std::endl;
        
        if (buffer_size < computeRTB2() || !is_sched)
        {
            src_task->add(&sync_set);
            task_set.add(src_task);
            
            dst_task->remove(&sync_set);
            
            auto task = std::find(task_set.getTs().begin(),
                                  task_set.getTs().end(), dst_task);
            if (task == task_set.getTs().end())
                delete dst_task;
        }
        else
        {
            auto task = std::find(task_set.getTs().begin(),
                                  task_set.getTs().end(), dst_task);
            if (task == task_set.getTs().end())
                task_set.add(dst_task);
        }
        
        buffer_size = computeRTB2();
        link = getLink();
    }
    std::cout << " - Buffer Size due to RT2 after optimizations: "
              << buffer_size << std::endl;
    
    return task_set;
}

std::vector<Function*> RtwOpt::getSS(Function* f)
{
    std::vector<Function*> tmp;
    tmp.push_back(f);
    getSyncSet(&tmp, f);
    return tmp;
}

long RtwOpt::getRT2()
{
    return computeRTB2();
}

