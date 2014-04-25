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

//  File: TaskSet.cpp

#include "TaskSet.h"
#include <algorithm>
#include <math.h>

bool tasksort(Task* a, Task* b)
{
    return ((a)->getPriority() < ((b)->getPriority()));
}

bool periodsort(Task* a, Task* b)
{
    return ((a)->getPeriod() < ((b)->getPeriod()));
}

TaskSet::TaskSet()
{
    
}

TaskSet::TaskSet(std::vector<Task*>& ts)
{
    for (auto i = ts.begin(); i != ts.end(); i++)
        taskset.push_back(*i);

    for (auto task : taskset)
    {
        if (task->getFunSize())
        {
            period = task->getPeriod();
            break;
        }
    }
	for (auto i = taskset.begin(); i != taskset.end(); i++)
        if ((*i)->getFunSize())
            period = gcd(period, (*i)->getPeriod());
    
    std::sort(taskset.begin(), taskset.end(), tasksort);
}

TaskSet::~TaskSet()
{
    
}

void TaskSet::computeFeta()
{
    
    places.clear();
    // Compute Period
    if (isEmpty())
    {
        places.clear();
        rbf.clear();
        return;
    }
    
    // Compute Hyperperiod
    long hyperperiod = 0;
    for (auto task : taskset)
    {
        if (task->getFunSize())
        {
            hyperperiod = task->getPeriod() * task->getSize();
            break;
        }
    }
    
	for (auto i = taskset.begin(); i != taskset.end(); i++)
    {
        if ((*i)->getFunSize())
        {
            long tmp1 = (*i)->getPeriod() * (*i)->getSize();
            hyperperiod = lcm(hyperperiod, tmp1);
        }
	}
    
    // Compute period
    for (auto task : taskset)
        period = gcd(period, task->getPeriod());
    for (auto task : taskset)
        if (task->getOffset() != 0)
            period = gcd(period, task->getOffset());
    
    long offset = 0;
    for (auto task : taskset)
        if (task->getOffset() > offset)
            offset = task->getOffset();
    
    for (auto i = 0; i < offset/period; i++)
        places.push_back(0);
    
    for (auto i = 0 ; i < hyperperiod/period; i++)
        places.push_back(0);
    hyperperiod = places.size() * period;
    
    for (auto i = places.begin(); i != places.end(); i++)
    {
        float wcet = 0;
        long t = (i - places.begin()) * period;
        
        for (auto i = taskset.begin(); i != taskset.end(); i++)
            if ((*i)->getFunSize() != 0)
                wcet += (*i)->getFeta(t);

        (*i) = wcet;
    }
    computeRbf();
    return;
}

void TaskSet::computeRbf()
{
    if (places.size() != 0)
    {
        for (auto t = 0; t <= (places.size() * period); t+= period)
        {
            float value = 0;
            for (auto task : taskset)
                value += task->getFeta(t);
            if (t == 0)
                rbf.push_back(value);
            else
                rbf.push_back(rbf.back() + value);
        }
    }
    //    std::cout << "RBF: " << std::endl;
    //    for (auto r : rbf)
    //        std::cout << r << " ";
    //    std::cout << std::endl;
    //
    //    if (places.size() != 0)
    //    {
    //        rbf.push_back(places[0]);
    //        for (auto i = ++places.begin(); i != places.end(); ++i)
    //            rbf.push_back(rbf.back() + (*i));
    //
    //    }
}

float TaskSet::getRbf(long t)
{
    auto hyperperiod = period * rbf.size();
    if (t < hyperperiod)
    {
        return rbf[t/period];
    }
    else
    {
        long index = long(t / period) % long(hyperperiod / period);
        long n = long(t/hyperperiod);
        return (n * rbf.back() + rbf[index]);
    }
}

bool TaskSet::isEmpty()
{
    for (auto task : taskset)
        
        if (task->getFunSize())
        {
            return false;
        }
    return true;
}


bool TaskSet::checkInterval(long s, long e)
{
    long start = s;
    long intermedie = s + period;
    for (auto i = s; i < e; i+=period)
    {
        if (getRbf(start) >= i && getRbf(start) <= (i+period))
            return true;
        start += period;
        intermedie += period;
    }
    return false;
}

std::vector<float> TaskSet::getRbf()
{
    return rbf;
}

bool TaskSet::checkSchedulability()
{
    std::vector<Task* > tmp_tasks;
    places.clear();
    
    // rbf is the RBF of the taskset I am checking for schedulability
    rbf.clear();
    
    // rbfs is the map(rbf, period) of the intermedie schedulability analysis
    //  results
    rbfs.clear();
    computeFeta();
    
    // Clear response time container and slacks container
    response_t.clear();
    slaks.clear();
    
    std::sort(taskset.begin(), taskset.end(), tasksort);
    
    for (auto i = taskset.begin(); i != taskset.end(); i++)
    {
        Task* this_task = (*i);
        tmp_tasks.push_back(this_task);
        
        TaskSet tmp_taskset(tmp_tasks);
        tmp_taskset.computeFeta();
        rbf = tmp_taskset.getRbf();
        period = tmp_taskset.getPeriod();
        
        std::pair<std::vector<float>, long> rbf2period;
        rbf2period = std::make_pair(rbf, tmp_taskset.getPeriod());
        rbfs.push_back(rbf2period);
        
        // Check schedulability, compute slacks and response times
        bool is_sched = tmp_taskset.computeResponseTimeO();
        
        // If the schedulability test fails, stop to check and returns false
        if (is_sched == false)
            return false;
        else
        {
            response_t.push_back(tmp_taskset.getResponseTimes().back());
            slaks.push_back(tmp_taskset.getMinSlack());
            //std::cout << "slack: " << tmp_taskset.getMinSlack() << std::endl;
        }
    }
    // If schedulability test suceded, return true
    return true;

}

bool TaskSet::computeResponseTimeO()
{
    
    
    // period: is the period of the FETA
    // this_task->getPeriod(): is the task period
    
    auto this_task = taskset.back();
    
#ifdef DEBUG_RT
    std::cout << std::endl << "Task: " << taskset.back()->getName() << std::endl;
    std::cout << "RBF: ";
    for (auto r : rbf)
        std::cout << r << " ";
    std::cout << std::endl;
    std::cout << "TASK feta: ";
    for (auto r : this_task->get())
        std::cout << r << " ";
    std::cout << std::endl;
    std::cout << "period: " << period << ", this_task->getPeriod(): " <<
    this_task->getPeriod() << std::endl;
#endif
    
    
    
    // Take all the time instants at which the rbf change value
    std::vector<long> time_instants;
    if (getRbf(0) != 0 && this_task->getOffset() == 0)
        time_instants.push_back(0);
    
    if (this_task->getOffset() != 0)
        time_instants.push_back(this_task->getOffset());
    
    for ( auto i = this_task->getOffset(); i < ( ( rbf.size() - 1 ) * period ); i += period )
        if (getRbf(i) != getRbf(i+period))
            time_instants.push_back(i+period);
    
    // I don't need to check the last point
    time_instants.pop_back();
    
    // interm_resp_t contains the response times for each busy period
    std::vector<float> interm_resp_t;
    
    if (time_instants.size() != 0)
    {
        // Time is the time line for each busy period:
        // - "time" is initialized to 0
        // - for successive busy periods, "time" is inizialized to the RBF value
        //      at the beginning of the busy period
        float time = 0;
        if (this_task->getOffset() != 0)
            time = getRbf(this_task->getOffset() - period);

        
        for (auto ti : time_instants)
        {
#ifdef DEBUG_RT
            std::cout << "ti: " << ti << std::endl;
#endif
            
            // Check for each busy period if there is the intersection between
            //  time and RBF
            
            
            long t = floor( (ti - this_task->getOffset()) / this_task->getPeriod() );
            t = t * this_task->getPeriod() + this_task->getOffset();
            if (t < ti)
                t += this_task->getPeriod();
            
            auto D = ti + (t - ti) + this_task->dstNext(t);
            
            for (auto i = ti; i <= D; i+=period)
            {
                
                time += period;
                
                float intersection = 0;
                bool found = false;
#ifdef DEBUG_RT
                std::cout << "i: " << i << std::endl;
                std::cout << "---getRbf(i): " << getRbf(i) <<
                ", (time-period): " << (time-period) << ", time: " << time
                << std::endl;
#endif
                if ( getRbf(i) > (time-period) && getRbf(i) <= time )
                {
#ifdef DEBUG_RT
                    std::cout << "+++getRbf(i): " << getRbf(i) <<
                    ", (time-period): " << (time-period) << ", time: " << time
                    << std::endl;
#endif
                    found = true;
                    
                    if (ti != 0)
                    {
                        intersection = ti + ( getRbf(i) - getRbf(ti-period) );
                    }
                    else
                    {
                        intersection = getRbf(i);
                    }
#ifdef DEBUG_RT
                    std::cout << "t: " << t << ", D: " << D << std::endl;
#endif
                    // The busy period is relevant only if "t" is inside the
                    //  busy period itself
                    if (intersection <= D && t >= ti && t < intersection)
                    {
#ifdef DEBUG_RT
                        std::cout << "Intersection: " << intersection <<
                        ", resp_time: " << intersection - t << ", slack: " <<
                        D - intersection << std::endl;
#endif
                        interm_resp_t.push_back( intersection - t );
                        slaks.push_back( D - intersection );
                        
                        // If negative slack (at this point the slack cannot be
                        // negative)
                        if ( ( D - intersection ) < 0 )
                        {
                            std::cout << "ERROR, negative slack" << std::endl;
                            
                            std::cout << std::endl << "Task: " <<
                            taskset.back()->getName() << std::endl;
                            
                            std::cout << "period: " << period <<
                            ", this_task->getPeriod(): " <<
                            this_task->getPeriod() << std::endl;
                            
                            std::cout << "RBF: ";
                            for (auto r : rbf)
                                std::cout << r << " ";
                            std::cout << std::endl;
                            
                            std::cout << "TASK feta: ";
                            for (auto r : this_task->get())
                                std::cout << r << " ";
                            std::cout << std::endl;
                            
                            std::cout << "ti: " << ti << std::endl;
                            std::cout << "t: " << t << ", D: " << D << std::endl;
                            
                            std::cout << "getRbf(i): " << getRbf(i) <<
                            ", (time-period): " << (time-period) << ", time: "
                            << time << std::endl;
                            
                            std::cout << "Intersection: " << intersection <<
                            ", resp_time: " << intersection - t << ", slack: "
                            << D - intersection << std::endl;
                            
                            exit(-1);
                        }
                        
                        // If negative response time (at this point the response
                        // time cannot be negative)
                        if ( (intersection - t ) <= 0 )
                        {
                            std::cout << "ERROR, negative respt" << std::endl;
                            
                            std::cout << std::endl << "Task: " <<
                            taskset.back()->getName() << std::endl;
                            
                            std::cout << "period: " << period <<
                            ", this_task->getPeriod(): " <<
                            this_task->getPeriod() << std::endl;
                            
                            std::cout << "RBF: ";
                            for (auto r : rbf)
                                std::cout << r << " ";
                            std::cout << std::endl;
                            
                            std::cout << "TASK feta: ";
                            for (auto r : this_task->get())
                                std::cout << r << " ";
                            std::cout << std::endl;

                            std::cout << "ti: " << ti << std::endl;
                            std::cout << "t: " << t << ", D: " << D << std::endl;
                            
                            std::cout << "getRbf(i): " << getRbf(i) <<
                            ", (time-period): " << (time-period) << ", time: "
                            << time << std::endl;
                            
                            std::cout << "Intersection: " << intersection <<
                            ", resp_time: " << intersection - t << ", slack: "
                            << D - intersection << std::endl;
                            
                            exit(-1);
                        }
                        
                    }
                    else if (intersection > D && t > ti)
                    {
                        //std::cout << "==> intersection > D && t > ti" << std::endl;
                        return false;
                    }
                    break;
                }
                if (found == false && time > D)
                {
                    //std::cout << "==> found = false" << std::endl;
                    return false;
                }
            }
            time = getRbf(ti);
        }
    }
    
    auto wcrt = *std::max_element(interm_resp_t.begin(),
                                  interm_resp_t.end());
    response_t.push_back(wcrt);
    return true;
}

std::vector<float> TaskSet::getResponseTimes()
{
    return response_t;
}

float TaskSet::getResponseTime(Function *f)
{
    TaskSet tmp_ts;
    Task *the_task = nullptr;
    long idx = 1;
    for (auto task : taskset)
    {
        if (task->isMy(f))
        {
            the_task = task;
            break;
        }
        idx++;
    }
    
    // I did't find the function in the taskset
    if (the_task == nullptr)
    {
        std::cout << "ERROR: runnable not in the taskset" << std::endl;
        exit(-1);
    }
    
    // Add to the TaskSet all the tasks at higher priority than "the_task"
    auto the_priority = the_task->getPriority();
    for (auto task : taskset)
        if (task->getPriority() < the_priority)
            tmp_ts.add(task);
    
    // Now create the task with function f and with all the tasks at higher
    //  execution order than f itself
    Task tmp_task;
    auto fun = the_task->getFunctions().begin();
    while (*fun != f)
    {
        tmp_task.add(*fun);
        fun++;
    }
    tmp_task.add(f);
    tmp_task.changePriority(the_priority);
    tmp_task.setOffset(the_task->getOffset());
    tmp_ts.add(&tmp_task);
    
    bool is_sched = tmp_ts.checkSchedulability();
    
    // Returns -1 if the TaskSet is not schedulable
    if (is_sched == false)
        return -1;
    // Else, returns the response time of the runnable f
    else
        return tmp_ts.getResponseTimes().back();
}

float TaskSet::getCompletionTime(Function * f)
{
    auto task = getTask(f);
    if (task != nullptr)
        return ( getResponseTime(f) + getTask(f)->getOffset() );
    else
        return -1;
}

std::vector<float> TaskSet::getSlack()
{
    return slaks;
}

float TaskSet::getMinSlack()
{
    if (taskset.size() == 0)
        return -1;
        
    auto tmp_slack = slaks;
    std::sort(tmp_slack.begin(), tmp_slack.end());
    return tmp_slack.front();
}

void TaskSet::printTs(std::ostream &stream)
{
    for (auto i = taskset.begin(); i != taskset.end(); i++)
        (*i)->print(stream);
}

void TaskSet::printTsExt(std::ostream &stream)
{
    for (auto i = taskset.begin(); i != taskset.end(); i++)
        (*i)->printExt(stream);
}

void TaskSet::printRbfs(std::ostream &stream)
{
    long i = rbfs.size();
    for (auto j = rbfs.begin(); j != rbfs.end(); j++) {
        stream << "RBF_" << i << "\t" << "t_" << i << "\t";
        i--;
    }
    stream << std::endl;
    
    i = 0;
    while (1) {
        bool someone = false;
        std::vector<float> last0;
        std::vector<long> last1;
        for (auto k = rbfs.rbegin(); k != rbfs.rend(); k++) {
            try {
                stream << (*k).first.at(i) << "\t" << i*(*k).second << "\t";
                someone = true;
                last0.push_back((*k).first.at(i));
                last1.push_back((*k).second);
            } catch (...) {}
        }
        stream << std::endl;
        long j = 0;
        for (auto k = last0.begin(); k != last0.end(); k++) {
            stream << (*k) << "\t" << (i+1)*last1.at(j++) << "\t";
        }
        stream << std::endl;
        i++;
        if (!someone)
            break;
    }
}

std::vector<Task* >& TaskSet::getTs()
{
    return taskset;
}

void TaskSet::add(Feta *t)
{
    if (dynamic_cast<Task*>(t))
    {
        Task * task = dynamic_cast<Task*>(t);
        auto i = std::find(taskset.begin(), taskset.end(), task);
        if (i == taskset.end())
        {
            taskset.push_back(task);
            std::sort(taskset.begin(), taskset.end(), tasksort);
        }
        
        // Compute Period
        for (auto task : taskset)
        {
            if (task->getFunSize())
            {
                period = task->getPeriod();
                break;
            }
        }
        for (auto i = taskset.begin(); i != taskset.end(); i++)
            if ((*i)->getFunSize())
                period = gcd(period, (*i)->getPeriod());
    }
}

void TaskSet::addFunction(Function *f)
{
    bool allocated = false;
    
    for (auto task : taskset)
    {
        if (task->getPeriod() == f->getPeriod())
        {
            task->add(f);
            allocated = true;
        }
    }
    
    if (allocated == false)
    {
        // Create the new task
        Task* new_task = new Task();
        new_task->add(f);
        taskset.push_back(new_task);
        
        // Re-assign priorities
        // Get tasks with lower period
        for (auto task : taskset)
            if (task->getPeriod() > f->getPeriod())
                task->changePriority(task->getPriority() + 1);
        
        std::vector<long> priorities;
        for (auto task : taskset)
            if (task->getPeriod() < f->getPeriod())
                priorities.push_back(task->getPriority());
        
        if (priorities.size() == 0)
            priorities.push_back(0);
        
        auto priority = (*(std::max_element(priorities.begin(), priorities.end()))) + 1;
        
        // Set the priority of the new task
        new_task->changePriority(priority);
    }
    
    // Compute Period
    for (auto task : taskset)
    {
        if (task->getFunSize())
        {
            period = task->getPeriod();
            break;
        }
    }
	for (auto i = taskset.begin(); i != taskset.end(); i++)
        if ((*i)->getFunSize())
            period = gcd(period, (*i)->getPeriod());
    
    std::sort(taskset.begin(), taskset.end(), tasksort);
}

void TaskSet::addFunction(Function *f, float offset)
{
    bool allocated = false;
    
    // If already exist a task running at the same period of function "f"
    //  add f to this task
    for (auto task : taskset)
    {
        if (task->getPeriod() == f->getPeriod())
        {
            task->add(f);
            
            // Change the offset of the task if the previous offset is smaller
            //  than the new one
            if (task->getOffset() < offset)
                task->setOffset(offset);
            
            allocated = true;
        }
    }
    
    // The runnable has not been allocated on previous tasks and a new task
    //  must be created for it
    if (allocated == false)
    {
        // Decrease priority of tasks with higher period
        for (auto task : taskset)
            if (task->getPeriod() > f->getPeriod())
                task->changePriority(task->getPriority() + 1);
        
        // Create the new task
        Task* new_task = new Task();
        new_task->add(f);
        taskset.push_back(new_task);
        
        // Set the offset for the new task
        new_task->setOffset(offset);
        
        // Re-assign priorities
        // Get tasks with lower period
        for (auto task : taskset)
            if (task->getPeriod() > f->getPeriod())
                task->changePriority(task->getPriority() + 1);
        
        std::vector<long> priorities;
        for (auto task : taskset)
            if (task->getPeriod() < f->getPeriod())
                priorities.push_back(task->getPriority());
        
        if (priorities.size() == 0)
            priorities.push_back(0);
        
        auto priority = (*(std::max_element(priorities.begin(), priorities.end()))) + 1;
        
        // Set the priority of the new task
        new_task->changePriority(priority);
    }

    // Compute Period
    for (auto task : taskset)
    {
        if (task->getFunSize())
        {
            period = task->getPeriod();
            break;
        }
    }
	for (auto i = taskset.begin(); i != taskset.end(); i++)
        if ((*i)->getFunSize())
            period = gcd(period, (*i)->getPeriod());
    
    std::sort(taskset.begin(), taskset.end(), tasksort);
}

float TaskSet::addFunction(Function *f, Processor* p)
{
    bool allocated = false;
    
    // If already exist a task running at the same period of function "f"
    //  add f to this task
    for (auto task : taskset)
    {
        if (task->getPeriod() == f->getPeriod())
        {
            task->add(f);
            
            // Change the offset of the task if the previous offset is smaller
            //  than the new one
            if (task->getOffset() < offset)
                task->setOffset(offset);
            
            allocated = true;
        }
    }
    
    // The runnable has not been allocated on previous tasks and a new task
    //  must be created for it
    if (allocated == false)
    {
        // Decrease priority of tasks with higher period
        for (auto task : taskset)
            if (task->getPeriod() > f->getPeriod())
                task->changePriority(task->getPriority() + 1);
        
        // Create the new task
        Task* new_task = new Task();
        new_task->add(f);
        taskset.push_back(new_task);
        
        // Set the offset for the new task
        new_task->setOffset(offset);
        
        // Re-assign priorities
        // Get tasks with lower period
        for (auto task : taskset)
            if (task->getPeriod() > f->getPeriod())
                task->changePriority(task->getPriority() + 1);
        
        std::vector<long> priorities;
        for (auto task : taskset)
            if (task->getPeriod() < f->getPeriod())
                priorities.push_back(task->getPriority());
        
        if (priorities.size() == 0)
            priorities.push_back(0);
        
        auto priority = (*(std::max_element(priorities.begin(), priorities.end()))) + 1;
        
        // Set the priority of the new task
        new_task->changePriority(priority);
    }
    
    // Compute Period
    for (auto task : taskset)
    {
        if (task->getFunSize())
        {
            period = task->getPeriod();
            break;
        }
    }
	for (auto i = taskset.begin(); i != taskset.end(); i++)
        if ((*i)->getFunSize())
            period = gcd(period, (*i)->getPeriod());
    
    std::sort(taskset.begin(), taskset.end(), tasksort);
    return 0;
}

void TaskSet::removeFunction(Function *f)
{
    bool erased = false;
    for (auto task : taskset)
    {
        if (task->removeC(f) == true)
        {
            erased = true;
            if (task->getFunSize() == 0)
            {
                taskset.erase(std::find(taskset.begin(), taskset.end(), task));
                delete task;
            }
        }
    }
    if (erased == false)
    {
        std::cout << "ERROR:, runnable " << f->getName() << " not allocated "
        << "in this taskset" << std::endl;
        exit(-1);
    }
    // Re-assign priorities
    std::sort(taskset.begin(), taskset.end(), tasksort);
    long p = 1;
    for (auto task : taskset)
        task->changePriority(p++);
    
    // Compute Period
    for (auto task : taskset)
    {
        if (task->getFunSize())
        {
            period = task->getPeriod();
            break;
        }
    }
	for (auto i = taskset.begin(); i != taskset.end(); i++)
        if ((*i)->getFunSize())
            period = gcd(period, (*i)->getPeriod());
    
    std::sort(taskset.begin(), taskset.end(), tasksort);
}

void TaskSet::remove(Feta *t)
{
    if (dynamic_cast<Task*>(t))
    {
        Task * task = dynamic_cast<Task*>(t);
        
        auto task_iterator = std::find(taskset.begin(), taskset.end(), task);
        if (task_iterator != taskset.end())
        {
            taskset.erase(task_iterator);
            computeFeta();
        }
        
        for (auto task : taskset)
        {
            if (task->getFunSize())
            {
                period = task->getPeriod();
                break;
            }
        }
        for (auto i = taskset.begin(); i != taskset.end(); i++)
            if ((*i)->getFunSize())
                period = gcd(period, (*i)->getPeriod());
        
        std::sort(taskset.begin(), taskset.end(), tasksort);
    }
}

void TaskSet::moveFunctions(std::vector<Function*> *f, Task *t)
{
    for (auto i = taskset.begin(); i != taskset.end(); i++)
        (*i)->remove(f);
    t->add(f);
    computeFeta();
}

long TaskSet::getPriority(Function *f)
{
    for (auto k = taskset.begin(); k != taskset.end(); k++)
    {
        auto i = std::find((*k)->getFunctions().begin(),
                           (*k)->getFunctions().end(), f);
        if (i != (*k)->getFunctions().end())
            return (*k)->getPriority();
    }
    throw "This function is not in the task set";
}

Task* TaskSet::getTask(long p)
{
    for (auto i = taskset.begin(); i != taskset.end(); i++) {
        if ((*i)->getPriority() == p)
            return *i;
    }
    return nullptr;
}

Task* TaskSet::getTask(Function *f)
{
    for (auto i = taskset.begin(); i != taskset.end(); i++)
        if ((*i)->isMy(f))
            return *i;
    return nullptr;
}

bool TaskSet::isMy(Task *t)
{
    for (auto task : taskset)
        if (task == t) return true;
    return false;
}

bool TaskSet::isMy(Function *f)
{
    for (auto task : taskset)
        if (task->isMy(f)) return true;
    return false;
}

void TaskSet::setRMPriorities()
{
    std::sort(taskset.begin(), taskset.end(), periodsort);
    auto priority = 1;
    for (auto task: taskset)
        task->changePriority(priority++);
    std::sort(taskset.begin(), taskset.end(), tasksort);
}


void TaskSet::adjustPriorities()
{
    std::map<long, long> tmp_p;
    std::sort(taskset.begin(), taskset.end(), tasksort);
    
    long p = 1;
    for (auto task : taskset)
        task->changePriority(p++);
}

void TaskSet::computeRT()
{
    total_size = 0;
    rt2_size = 0;
    rt1_size = 0;
    
    // For each task
    for (auto task : taskset)
    {
        // For each function in the task
        for (auto function : task->getFunctions())
        {
            // for each successor of the function
            for (auto succ : function->getSuccessors())
            {
                // RT1 blocks
                // It should be added the additional condition upon the response
                // time of the receiver:
                // {&& responseTime(succ) < period(function)}
                if (isMy(succ.first) && getPriority(succ.first) > getPriority(function))
                {
                    rt1_size += succ.second;
                    
                    // If the periods are not harmonic, add a RT2
                    if (succ.first->getPeriod() % function->getPeriod() != 0)
                        rt2_size += (2 * succ.second);
                }
                // RT2 blocks
                if (isMy(succ.first) && getPriority(succ.first) < getPriority(function))
                {
                    rt2_size += (2 * succ.second);
                    
                    // If the periods are not harmonic, add a RT1
                    if (function->getPeriod() % succ.first->getPeriod() != 0)
                        rt1_size += succ.second;
                }
            }
        }
    }
    
    total_size = rt1_size + rt2_size;
}

long TaskSet::getRT1()
{
    return rt1_size;
}

long TaskSet::getRT2()
{
    return rt2_size;
}

long TaskSet::getRT()
{
    return total_size;
}

float TaskSet::getLoad()
{
    computeFeta();
    if (places.size() == 0)
        return 0;
    float load = 0;
    for (auto p : places)
        load = load + (p * period);
    return (load / (places.size() * period) );
}


