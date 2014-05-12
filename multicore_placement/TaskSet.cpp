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

//#define DEBUG_RT


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
    last_link_size = -1;
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
    
    last_link_size = -1;
    
}

TaskSet::~TaskSet()
{
    
}

TaskSet::TaskSet(const TaskSet& other)
{
	for(int i=0; i< other.taskset.size(); i++)
	{
		Task *t = new Task(*other.taskset[i]);
		taskset.push_back(t);
	}
	period = other.period;
	places = other.places;
	rbf = other.rbf;
	rbfs = other.rbfs;
	response_t = other.response_t;
	slaks = other.slaks;
	total_size = other.total_size;
	rt2_size = other.rt2_size;
	rt1_size = other.rt1_size;
}

TaskSet& TaskSet::operator = (const TaskSet& other)
{
	if(!taskset.empty())
	{
		for(int i=0;i<taskset.size();i++)
		{
			delete taskset[i];
		}
		taskset.clear();
	}
    
	for(int i=0; i < other.taskset.size(); i++)
	{
		Task *t = new Task(*other.taskset[i]);
		taskset.push_back(t);
	}
	period = other.period;
	places = other.places;
	rbf = other.rbf;
	rbfs = other.rbfs;
	response_t = other.response_t;
	slaks = other.slaks;
	total_size = other.total_size;
	rt2_size = other.rt2_size;
	rt1_size = other.rt1_size;
	return *this;
}

void TaskSet::getSyncSet(std::vector<Function*>*sync_set, Function *f)
{
    for (auto i = f->getPredecessors().begin();
         i != f->getPredecessors().end(); i++)
    {
        auto check = std::find(sync_set->begin(), sync_set->end(), (*i).first);
        if (f->getPeriod() == (*i).first->getPeriod() &&
            check == sync_set->end())
        {
            if (isMy((*i).first))
            {
                sync_set->push_back((*i).first);
                getSyncSet(sync_set, (*i).first);
            }
        }
    }
    
    for (auto i = f->getSuccessors().begin();
         i != f->getSuccessors().end(); i++)
    {
        auto check = std::find(sync_set->begin(), sync_set->end(), (*i).first);
        if (f->getPeriod() == (*i).first->getPeriod() &&
            check == sync_set->end())
        {
            if (isMy((*i).first))
            {
                sync_set->push_back((*i).first);
                getSyncSet(sync_set, (*i).first);
            }
        }
    }
}

std::pair<Function*, Function*> TaskSet::getLink()
{
 
    long width = -1;
    std::pair<Function*, Function*> tmp_link = std::make_pair(nullptr, nullptr);
    
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
    
    else
    {
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
    
//    std::cout << "Taskset: " << this << std::endl;
//    for (auto l : rt2s)
//        std::cout << l.first.first->getName() << " -> "
//        << l.first.second->getName() << std::endl;
    
    
    return tmp_link;
}

void TaskSet::optimize(Function* s, Function* d)
{
    src_funs.clear();
    dst_funs.clear();
    
    // Get the synchronous set of the source function
    std::vector<Function*> sync_set;
    sync_set.push_back(s);
    getSyncSet(&sync_set, s);
    
    // Get the task where the Synchronous Set is mapped
    src_task = getTask(s);
    
    // Get the task where the Synchronous Set will be mapped
    dst_task = getTask(d);
    
    if (src_task == nullptr || dst_task == nullptr)
    {
        std::cout << "null task: - src: " << s->getName()
        << " - dst: " << d->getName() << std::endl;
        std::cout << "Functions: " << std::endl;
        for (auto f : functions)
            std::cout << f->getName() << std::endl;
        exit(-1);
    }
    
    if (s != nullptr && d != nullptr)
//        && src_task != nullptr && dst_task != nullptr)
    {
        auto funs0 = src_task->getFunctions();
        for (auto f : funs0)
            src_funs.push_back(f);
        
        // Remove the Synchronous Set from the task.
        src_task->remove(&sync_set);
        
        // Remove the task from the taskset if it is empty
        if (src_task->getFunSize() == 0)
        {
            auto f = std::find(taskset.begin(), taskset.end(), src_task);
            taskset.erase(f);
        }
        

        // Save the origilal function configuration of dst_task
        auto funs1 = dst_task->getFunctions();
        for (auto f : funs1)
            dst_funs.push_back(f);
        
        // Add the Synchronous Set to the destination task
        dst_task->add(&sync_set);
    }
    //adjustPriorities();
}

void TaskSet::undoOptimize()
{

    // If the original task is empty, re-add it to the taskset
    if (src_task->getFunSize() == 0)
    {
        //add(src_task);
        taskset.push_back(src_task);
        std::sort(taskset.begin(), taskset.end(), tasksort);
    }
    
    // Add the SynchronousSet to the original task
    src_task->replaceFunctions(src_funs);
    
    // Remove the Synchronous Set from the current task
    dst_task->replaceFunctions(dst_funs);

    //adjustPriorities();
}

void TaskSet::confirmOptimization()
{
//    if (src_task->getFunSize() == 0)
//        delete src_task;
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
        for (auto t = 0; t <= (2 * places.size() * period); t+= period)
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
}

float TaskSet::getRbf(long t)
{
    if (t >= rbf.size()*period)
    {
        float rbf = 0;
        for (auto time = 0; time <= t; time += period)
            for (auto task : taskset)
                rbf += task->getFeta(time);
        return rbf;
    }
    
    return rbf[int(t/period)];
}

std::vector<float> TaskSet::getRbf()
{
    return rbf;
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
    std::cout << "period: " << period << ", this_task->getPeriod(): "
    << this_task->getPeriod() << std::endl;
#endif
    
    // Take all the time instants at which the rbf change value
    std::vector<long> time_instants;
    if (getRbf(0) != 0 && this_task->getOffset() == 0)
        time_instants.push_back(0);
    
    if (this_task->getOffset() != 0)
        time_instants.push_back(this_task->getOffset());
    
    for ( auto i = this_task->getOffset();
         i <= ( ( places.size() - 1 ) * period ); i += period )
        if (getRbf(i) != getRbf(i+period))
            time_instants.push_back(i+period);
    
    bool is_no_offset = true;
    for (auto task : taskset)
    {
        if (task->getOffset() != 0)
        {
            is_no_offset = false;
            break;
        }
    }
    
    std::vector<long> tmp_ti;
    // If the task set is offset free, I have to check up to task period
    if (is_no_offset == true)
    {
        for (auto t = time_instants.begin(); t != time_instants.end(); t++)
        {
            if (*t <= this_task->getPeriod())
            {
                tmp_ti.push_back(*t);
            }
        }
        time_instants = tmp_ti;
    }
    
    // I don't need to check the last point
    else
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
            
            
            long t = floor((ti - this_task->getOffset()) /
                           this_task->getPeriod());
            t = t * this_task->getPeriod() + this_task->getOffset();
            if (t < ti)
                t += this_task->getPeriod();
            
            float D = ti + (t - ti) + this_task->dstNext(t);
#ifdef DEBUG_RT
            std::cout << "D: " << D << std::endl;
#endif
            for (auto i = ti; i < D; i+=period)
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
                if ( getRbf(i) <= time )
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
                        return false;
                    }
                    break;
                }
                
                if (found == false && (i + period) >= D)
                {
                    return false;
                }
            }
            time = getRbf(ti);
        }
    }
    
    float wcrt = 0.0;
    if (interm_resp_t.size() == 0)
        return false;
    else
        wcrt = *std::max_element(interm_resp_t.begin(),
                                  interm_resp_t.end());
    //std::cout << wcrt << std::endl;
    response_t.push_back(wcrt);
    return true;
}

std::pair<float, long> TaskSet::testRunnable(Function* runnable)
{
    std::pair<float, long> st_p; // Start time - Priority
    long this_period = runnable->getPeriod();
    long this_priority = 0;
    Task* hp_task = nullptr;
    
    // Take the lowest priority between those if the tasks with the same period
    for (auto task : taskset)
    {
        if (task->getPeriod() <= this_period)
        {
            this_priority = task->getPriority();
            hp_task = task;
        }
    }
    if (this_priority != 0)
    {
        auto cp_hp_task = getCompletionTime(hp_task->getFunctions().back());
        st_p = std::make_pair(cp_hp_task, this_priority);
        return st_p;
    }
    else
    {
        st_p = std::make_pair(0.0, 1);
        return st_p;
    }
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
    auto fun_list = the_task->getFunctions();
    for (auto fun = fun_list.begin(); fun != fun_list.end(); fun++)
    {
        tmp_task.add(*fun);
        if ((*fun) == f)
            break;
    }
    
    tmp_task.changePriority(the_priority);
    tmp_task.setOffset(the_task->getOffset());
    tmp_ts.add(&tmp_task);
    
    bool is_sched = tmp_ts.checkSchedulability();
    
    // Returns -1 if the TaskSet is not schedulable
    if (is_sched == false)
        return -1;
    // Else, returns the response time of the runnable f
    else
    {
        //std::cout << "RespT: " << tmp_ts.getResponseTimes().back() << std::endl;
        return tmp_ts.getResponseTimes().back();
    }
}

float TaskSet::getCompletionTime(Function * f)
{
    auto task = getTask(f);
    if (task != nullptr)
    {
        auto ct = getResponseTime(f);
        auto of = getTask(f)->getOffset();
        return ( ct + of );
    }
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
    
    Task* the_task = nullptr;
    
    // Determine the lowest priority task with the same period of the runnable
    for (auto task : taskset)
        if (task->getPeriod() == f->getPeriod())
            the_task = task;
    
    if (the_task != nullptr)
    {
        the_task->add(f);
        allocated = true;
    }
    
    if (allocated == false)
    {
        // Create the new task
        Task* new_task = new Task();
        new_task->add(f);
        taskset.push_back(new_task);
        
        // Decrease priority of tasks with higher period
        for (auto task : taskset)
            if (task->getPeriod() > f->getPeriod())
                task->changePriority(task->getPriority() + 1);
        
        // Compute the priority of the new task
        std::vector<long> priorities;
        for (auto task : taskset)
            if (task->getPeriod() <= f->getPeriod())
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
    
    
    Task* the_task = nullptr;
    
    // Determine the lowest priority task with the same period of the runnable
    for (auto task : taskset)
        if (task->getPeriod() == f->getPeriod())
            the_task = task;
    
    if (the_task != nullptr)
    {
        the_task->add(f);
        if (the_task->getOffset() < offset)
            the_task->setOffset(offset);
        allocated = true;
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
        new_task->setOffset(offset);
        
        // Compute the priority of the new task
        std::vector<long> priorities;
        for (auto task : taskset)
            if (task->getPeriod() <= f->getPeriod())
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

void TaskSet::addFunction(Function *f, long max_p)
{
    bool allocated = false;
    
    Task* the_task = nullptr;
    
    // Determine the lowest priority task with the same period of the runnable
    for (auto task : taskset)
        if (task->getPriority() == (max_p+1))
            the_task = task;
    
    if (the_task != nullptr)
    {
        the_task->add(f);
        allocated = true;
    }
    
    if (allocated == false)
    {
        // Create the new task
        Task* new_task = new Task();
        new_task->add(f);
        new_task->changePriority(max_p+1);
        taskset.push_back(new_task);
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

void TaskSet::addFunction(Function *f, float offset, long max_p)
{
    bool allocated = false;
    
    
    Task* the_task = nullptr;
    
    // Determine the lowest priority task with the same period of the runnable
    for (auto task : taskset)
        if (task->getPriority() == (max_p+1))
            the_task = task;
    
    if (the_task != nullptr)
    {
        the_task->add(f);
        if (the_task->getOffset() < offset)
            the_task->setOffset(offset);
        allocated = true;
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
        new_task->changePriority(max_p+1);
        taskset.push_back(new_task);
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

    period = 0;
    places.clear();
    rbf.clear();
    rbfs.clear();
    slaks.clear();
    response_t.clear();
    
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
    
    computeFeta();
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
        if (task->isMy(f) == true)
            return true;
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
    std::sort(taskset.begin(), taskset.end(), tasksort);
    
    long p = 1;
    for (auto task : taskset)
        task->changePriority(p++);
}

void TaskSet::refreshFunctions()
{
    functions.clear();
    for (auto task : taskset)
    {
        auto FUNCTIONS = task->getFunctions();
        for (auto fun : FUNCTIONS)
            functions.push_back(fun);
    }
}

void TaskSet::computeRT()
{
    total_size = 0;
    rt2_size = 0;
    rt1_size = 0;
    
    rt2s.clear();
    
    // For each task
    for (auto task : taskset)
    {
        // For each function in the task
        for (auto function : task->getFunctions())
        {
            // for each successor of the function
            auto successors = function->getSuccessors();
            for (auto succ : successors)
            {
                // RT1 blocks
                // It should be added the additional condition upon the response
                // time of the receiver:
                // {&& responseTime(succ) < period(function)}
                Function* successor = succ.first;
                long size = succ.second;
                
                bool is_my = false; //isMy(successor);
                refreshFunctions();
                for (auto f : functions)
                    if (successor == f)
                        is_my = true;
                
                
                if (is_my &&
                    getPriority(successor) > getPriority(function))
                {
                    rt1_size += size;
                    
                    // If the periods are not harmonic, add a RT2
                    if (successor->getPeriod() % function->getPeriod() != 0)
                        rt2_size += (2 * size);
                }
                
                // RT2 blocks
                
                if (is_my &&
                    (getPriority(successor) < getPriority(function)))
                {
                    rt2_size += (2 * size);
                    
                    std::pair<Function*, Function*> link;
                    link = std::make_pair(function, successor);
                    rt2s[link] = size;
                    
                    // If the periods are not harmonic, add a RT1
                    if (function->getPeriod() % succ.first->getPeriod() != 0)
                        rt1_size += size;
                }
            }
        }
    }
    total_size = rt1_size + rt2_size;
    
//    std::cout << "Taskset: " << this << std::endl;
//    for (auto l : rt2s)
//        std::cout << l.first.first->getName() << " - > "
//        << l.first.second->getName() << std::endl;
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


void TaskSet::randomSwapPriorities()
{
	if (taskset.size() == 1)
	{
		return;
	}
	int p1 = rand()%taskset.size();
	int p2;
	do{
		p2 = rand()%taskset.size();
	}while(p1 == p2);
	
	//std::cout<<"p1: "<<p1<<"   p2: "<<p2<<"   taskset.size(): "<<taskset.size()<<std::endl;
	
	auto t1 = p1 + taskset.begin();
	auto t2 = p2 + taskset.begin();
	
	auto prio1 = (*t1)->getPriority();
	auto prio2 = (*t2)->getPriority();
	
	(*t1)->changePriority(prio2);
	(*t2)->changePriority(prio1);
	
	std::sort(taskset.begin(), taskset.end(), tasksort);
	
}

void TaskSet::randomPartition()
{
	//std::cout<<"Partition begins..."<<std::endl;
	//srand(1);
	//check if each of all the tasks has only one function, we do merge instead of parition.
	bool check = false;
	for(auto it = taskset.begin();it!=taskset.end();it++)
	{
		if((*it)->getFunSize() != 1)
		{
			check = true;
		}
	}
	if(!check)
	{
		randomMerge();
		//std::cout<<"each task has only one function..."<<std::endl;
		return;
	}
	auto src = taskset.begin()+rand()%taskset.size();
	int newFunSize = rand()%(*src)->getFunSize() + 1;
	while((*src)->getFunSize() == 1){
		src = taskset.begin()+rand()%taskset.size();
		newFunSize = rand()%(*src)->getFunSize() + 1;
	}
	if (newFunSize == (*src)->getFunSize())
		newFunSize = newFunSize - 1;
	std::vector<Function* > fs;
	(*src)->getFunctionSet(&fs,newFunSize);
	Task* new_task = new Task();
	new_task->add(&fs);
	//std::cout<<"partition bug..."<<std::endl;
	//(*this).printTs(std::cout);
	(*src)->remove(&fs);
	add(new_task);
	//set random priority for the new task
	//auto i = std::find(taskset.begin(), taskset.end(), new_task);
	int new_prio = rand()%taskset.size()+1;
	(*new_task).changePriority(new_prio);
	//(*i)->changePriority(new_prio);
	adjustPriorities();
	//std::cout<<"Partition ends..."<<std::endl;
	
}

void TaskSet::randomMerge()
{
	//std::cout<<"Merge begins..."<<std::endl;
	//(*this).printTsExt(std::cout);
	if(taskset.size() == 1)
	{
		//std::cout<<"critical situation.....!!!"<<std::endl;
		//auto i = taskset.begin();
		/*if((*i)->getFunSize() != 1)
         {
         randomPartition();
         }*/
		return;
	}
	int p1 = rand()%taskset.size();
	int p2;
	
	do{
		p2 = rand()%taskset.size();
	}while(p1 == p2);
	
	auto t1 = p1 + taskset.begin();
	auto t2 = p2 + taskset.begin();
	
	std::vector<Function* > fs;
	fs = (*t2)->getFunctions();
	
	(*t1)->add(&fs);
	
	//(*t2)->remove(&fs);
	
	remove(*t2);
	//(*this).printTs(std::cout);
	//std::sort(taskset.begin(), taskset.end(), tasksort);
	adjustPriorities();
	//std::cout<<"Merge ends..."<<std::endl;
}

long TaskSet::getTaskSetSize()
{
	return taskset.size();
}




