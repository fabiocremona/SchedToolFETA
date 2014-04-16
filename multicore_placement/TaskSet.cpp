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
        {
            if ((*i)->getFunSize() != 0)
            {
                wcet += (*i)->getFeta(t);
                //std::cout << "Feta at t: " << t << " : " << (*i)->getFeta(t) << std::endl;
            }
        }
        (*i) = wcet;
    }
    
//    std::cout << "Taskset FETA: " << std::endl;
//    for (auto f : places)
//        std::cout << f <<" ";
//    std::cout << std::endl;
    
//    std::cout << "Feta-" << std::endl;
//    for (auto t = 0; t <= 26; t+=2)
//        std::cout << getFeta(t) << " ";
//    std::cout << std::endl;
    
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
    //std::cout << "Period: " << period << " Hyperperiod: " << hyperperiod << std::endl;
    //std::cout << "t: " << t << std::endl;
    if (t < hyperperiod)
    {
        return rbf[t/period];
    }
    else
    {
        long index = long(t / period) % long(hyperperiod / period);
        long n = long(t/hyperperiod);
        return (n*rbf.back() + rbf[index]);
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
//        std::cout << "RBF at: " << start << " = " << getRbf(start) << " time: " << i
//        << " RBF at: " << intermedie << " = " << getRbf(start) << " time: " << (i+period) << std::endl;
        
        if (getRbf(start) >= i && getRbf(start) <= (i+period))
            return true;
        start += period;
        intermedie += period;
    }
    return false;
    
//    for (auto i = s; i < e; i+=period)
//    {
//        //auto hyperperiod = period * rbf.size();
//        auto index = long(i / period);// % long(hyperperiod / period);
//        if (rbf[index] > e)
//        {
//            return false;
//        }
//    }
//    return true;
}

std::vector<float> TaskSet::getRbf()
{
    return rbf;
}

bool TaskSet::checkSchedulability()
{
    std::vector<Task* > tmp_tasks;
    bool is_schedulable = true;
    places.clear();
    rbf.clear();
    rbfs.clear();
    
    bool with_offset = false;
    for (auto task : taskset)
        if (task->getOffset() != 0)
            with_offset = true;
    
    if (with_offset == false)
    {
        std::sort(taskset.begin(), taskset.end(), tasksort);
        for (auto i = taskset.begin(); i != taskset.end(); i++)
        {
            Task* this_task = (*i);
            tmp_tasks.push_back(this_task);
            
            TaskSet tmp_taskset(tmp_tasks);
            tmp_taskset.computeFeta();
            rbf = tmp_taskset.getRbf();
            
            std::pair<std::vector<float>, long> rbf2period;
            rbf2period = std::make_pair(rbf, tmp_taskset.getPeriod());
            rbfs.push_back(rbf2period);
            
            // For offset free tasks, it is enough to check the interval
            // [0, to the next firing location]
            is_schedulable = tmp_taskset.checkInterval(0, this_task->dstNext(0));
            if (!is_schedulable)
                break;
        }
        
        // rbf is the RBF of the TaskSet
        if (is_schedulable){
            computeResponseTime();
            return true;
        }
        
        else
        {
            slaks.clear();
            return false;
        }
    }
    
    else
    {
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

            auto resp_t = tmp_taskset.computeResponseTimeO();

            if (resp_t > this_task->getPeriod())
            {
                slaks.clear();
                response_t.clear();
                return false;
            }
            else
            {
                slaks.push_back( this_task->getPeriod() - resp_t );
                response_t.push_back(resp_t);
            }
        }
        return true;
    }
}

float TaskSet::computeResponseTimeO()
{
//    std::cout << "Task: " << taskset.back()->getName() << std::endl;
    
    // Take all the time instants at which the rbf change value
    std::vector<long> time_instants;


    if (getRbf(0) != 0)
        time_instants.push_back(0);
    
    for ( auto i = 0; i < ( (rbf.size()-1) * period ); i += period )
        if (getRbf(i) != getRbf(i+period))
            time_instants.push_back(i+period);
    
    std::vector<float> interm_resp_t;
    long time = 0;
    auto this_task = taskset.back();
    for (auto ti : time_instants)
    {
        
        bool cond = false;
        for (auto i = ti; i < ( ti + this_task->getPeriod() ); i+=period)
        {
            time += period;
            
            float intersection = 0;
            if ( getRbf(i) > (time-period) && getRbf(i) <= time )
            {
                cond = true;
                
                if (ti != 0)
                    intersection = ti + (getRbf(i) - getRbf(ti-period));
                else
                    intersection = getRbf(i);
                
                long t = (long)((intersection - this_task->getOffset() - period)
                                /this_task->getPeriod());
                t = t * this_task->getPeriod() + this_task->getOffset();
                
                if (t >= ti)
                    interm_resp_t.push_back(intersection - t);
                
                break;
            }
        }
        
        if (cond == false && (time - ti) >= this_task->getPeriod())
            interm_resp_t.push_back(rbf.size() * period);
        
        time = getRbf(ti);
        cond = false;
    }
    
//    std::cout << "Response times: ";
//    for (auto t : interm_resp_t)
//        std::cout << t << " ";
//    std::cout << std::endl;
    
    // If
    if (interm_resp_t.size() == 0)
        return (period * rbf.size() + 1);
    else
        return *std::max_element(interm_resp_t.begin(), interm_resp_t.end());
}

void TaskSet::computeResponseTime()
{
    response_t.clear();
    slaks.clear();
    
    if (taskset.size() == 0) return;
    for (auto r : rbfs)
    {
        auto idx = std::distance(rbfs.begin(), std::find(rbfs.begin(), rbfs.end(), r));
        long p = r.second;
        auto f = r.first;
        bool condition = true;
        for (auto i = f.begin(); i != f.end(); i++)
        {
            auto s = (i - f.begin()) * p;
            auto e = s + p;
            if (s <= *i && e >= *i && condition)
            {
                response_t.push_back(*i);
                slaks.push_back(taskset.at(idx)->getPeriod() - *i);
                condition = false;
            }
        }
    }
}

std::vector<float> TaskSet::getResponseTimes()
{
    return response_t;
}

float TaskSet::getResponseTime(Function *f)
{
//    for (auto task : taskset)
//    {
//        if (task->getOffset() != 0)
//        {
//            std::cout << "Error: used " << __FUNCTION__ << " with tasks with offset" << std::endl;
//            exit(-1);
//        }
//    }
    
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
        return 0;
    
    // Add to the TaskSet all the tasks at higher priority than "the_task"
    auto the_priority = the_task->getPriority();
    for (auto task : taskset)
        if (task->getPriority() < the_priority)
            tmp_ts.add(task);
    
    // Now create the task with function f and with all the tasks at higher
    // execution order than f itself
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

std::vector<float> TaskSet::getSlack()
{
    //return taskset.back()->getPeriod() - response_t.back();
    return slaks;
}

float TaskSet::getMinSlack()
{
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
    
    // Decrease priority of tasks with higher period
    for (auto task : taskset)
        if (task->getPeriod() > f->getPeriod())
            task->changePriority(task->getPriority() + 1);
    
    // Create the new task
    Task* new_task = new Task();
    new_task->add(f);
    taskset.push_back(new_task);
    
    // To add the offset to the feta, I simply add empty locations on top of
    // the FETA of the task. The feta of the function is left unchanged.
    new_task->setOffset(offset);
    
    
    
    // Take the tasks with equal period
    std::vector<Task*> tasks_ep;
    for (auto task : taskset)
        if (task->getPeriod() == f->getPeriod())
            tasks_ep.push_back(task);
    
    // Decrease the priority for tasks with same period but higher offset
    for (auto task : tasks_ep)
        if (task->getOffset() > offset)
            task->changePriority(task->getPriority() + 1);
    
    // Set the priority of the new task
    if (tasks_ep.size() != 0) {
        std::vector<long> priorities;
        for (auto task : tasks_ep)
            priorities.push_back(task->getPriority());
        auto priority = *std::min_element(priorities.begin(), priorities.end()) - 1;
        new_task->changePriority(priority);
    }
    else
    {
        std::vector<long> priorities;
        for (auto task : taskset)
            if (task->getPeriod() < f->getPeriod())
                priorities.push_back(task->getPriority());
        auto priority = *std::max_element(priorities.begin(), priorities.end()) + 1;
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

void TaskSet::removeFunction(Function *f)
{
    for (auto task : taskset)
        if (task->removeC(f))
            if (task->getFunSize() == 0)
            {
                taskset.erase(std::find(taskset.begin(), taskset.end(), task));
                delete task;
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

// Returns the list of tasks affine to task t in this taskset
std::vector<Task*> TaskSet::getAffineTasks(Task* t)
{
    std::vector<Task*> affine;
    
    for (auto function : t->getFunctions())
        for (auto f : function->getPredecessors())
            if (getTask(f.first) != nullptr) affine.push_back(getTask(f.first));
    return affine;
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



