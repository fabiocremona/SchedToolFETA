#include "Allocation.h"

Allocation::Allocation()
{
	//std::cout<<"allocation is constructed...."<<std::endl;
}

Allocation::Allocation(const Allocation& other)
{
	//std::cout<<"calling copy constructor......"<<std::endl;
	for(int i=0;i<other.cores.size();i++)
	{
		TaskSet *ts = new TaskSet(*other.cores[i]);
		cores.push_back(ts);
	}
	
	total_size = other.total_size;
	rt2_size = other.rt2_size;
	rt1_size = other.rt1_size;
}

Allocation& Allocation::operator = (const Allocation& other)
{	
	//std::cout<<"calling operator overloading...."<<std::endl;
	if(!cores.empty())
	{
		for(int i=0;i<cores.size();i++)
		{
			//std::cout<<"deleting cores: "<<std::endl;
			delete cores[i];
		}
		cores.clear();
	}
	
	for(int i=0;i<other.cores.size();i++)
	{
		TaskSet *ts = new TaskSet(*other.cores[i]);
		cores.push_back(ts);
	}
	
	total_size = other.total_size;
	rt2_size = other.rt2_size;
	rt1_size = other.rt1_size;
	
	return *this;
}

Allocation::Allocation(TaskSet* ts, int num_cores)
{
	for(int i=0;i<num_cores;i++)
		cores.push_back(new TaskSet());

	for(int i=0;i<(*ts).getTaskSetSize();i++)
	{
		int offset = i%num_cores;
		auto c = cores.begin() + offset;
		Task* t = new Task(*(*ts).getTask(i+1));
		(*c)->add(t);//get task by priority;
	}
	
	for(auto c = cores.begin();c!=cores.end();c++)
	{
		(*c)->adjustPriorities();
	}
}

Allocation::~Allocation()
{
	//std::cout<<"destruct allocation.........."<<std::endl;
	for(int i=0;i<cores.size();i++)
		delete cores[i];
	cores.clear();
}

std::vector<TaskSet*>& Allocation::getCores()
{
	return cores;
}

TaskSet* Allocation::getTaskSet(long offset)
{
	if(offset >= cores.size())
		return nullptr;
		
	auto ts_ptr = cores.begin()+offset;
	return *ts_ptr;
}

TaskSet* Allocation::getTaskSet(Function *f)
{
    for (auto core : cores)
        for (auto task : core->getTs())
            if (task->isMy(f))
                return core;
	//std::cout<<"couldn't find function in all task sets......"<<f->getName()<<std::endl;
	//printAllocation(std::cout);
    return nullptr;
}

void Allocation::computeRT()
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
    //Peng modified here, get all the runnables
	std::vector<Function* > runnables;
	for(auto ts: cores)
		for(auto t: ts->getTs())
			for(auto f: t->getFunctions())
				runnables.push_back(f);
	
    for (auto fun: runnables)
    {
        for (auto succ : fun->getSuccessors())
        {
            auto core_f = getTaskSet(fun);
            auto core_s = getTaskSet(succ.first);
            
            if (core_f != core_s)
            {
                if (fun->getPeriod() > succ.first->getPeriod())
                    rt1_size += succ.second;
                if (fun->getPeriod() < succ.first->getPeriod())
                    rt2_size += succ.second;
            }
        }
    }
	total_size = rt1_size + rt2_size;
}

long Allocation::getRT1()
{
    return rt1_size;
}

long Allocation::getRT2()
{
    return rt2_size;
}

long Allocation::getRT()
{
    return total_size;
}

int Allocation::getCoreNum()
{
	return cores.size();
}

std::map<Function*, float> Allocation::getResponseTimes()
{
    std::map<Function*, float> resp_t;
	
	std::vector<Function* > runnables;
	for(auto ts: cores)
		for(auto t: ts->getTs())
			for(auto f: t->getFunctions())
				runnables.push_back(f);
    //auto runnables = getAllocatedRunnables();
    for (auto r : runnables)
    {
        auto core = getTaskSet(r);
        auto r_t = core->getResponseTime(r);
        resp_t[r] = r_t;
		//std::cout<<"computing response time for runnable "<<r->getName()<<std::endl;
    }
    return resp_t;
}

std::map<Task*, long> Allocation::getOffsets()
{
    std::map<Task*, long> offsets;
    for (auto core : cores)
        for (auto task : core->getTs())
            offsets[task] = task->getOffset();
    return offsets;
}

std::map<Task*, float> Allocation::getNewOffsets()
{
    std::map<Task*, float> offsets_t;
    std::map<Function*, float> respt_f;
    
	std::vector<Function* > runnables;
	for(auto ts: cores)
		for(auto t: ts->getTs())
			for(auto f: t->getFunctions())
				runnables.push_back(f);
    //auto allocated_f = getAllocatedRunnables();
    
    for (auto f : runnables)
    {
        auto c_t = getTaskSet(f)->getCompletionTime(f);
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

std::map<Task*, float> Allocation::setNewOffsets()
{
    auto offsets = getNewOffsets();
    for (auto t : offsets)
        t.first->setOffset(t.second);
    return offsets;
}

void Allocation::setOffsets(std::map<Task*, float> offsets)
{
    for (auto t : offsets)
        t.first->setOffset(t.second);
}

void Allocation::clearOffsets()
{
	for(auto ts: cores)
		for(auto t:ts->getTs())
			t->setOffset(0);
}

void Allocation::moveTask(TaskSet *src, TaskSet *dst, Task* t)
{
	(*src).remove(t);
	(*src).adjustPriorities();
	
	(*dst).add(t);
	int new_prio = rand()%(*dst).getTaskSetSize() + 1;
	(*t).changePriority(new_prio);
	(*dst).adjustPriorities();
	
}

void Allocation::randomMoveAllocation()
{
	//std::cout<<"Allocation begins..."<<std::endl;
	if(cores.size() == 1)
	{
		return;
	}
	
	int a1 = rand()%cores.size();
	auto ts1 = a1 + cores.begin();
	while((*ts1)->getTaskSetSize() == 0)
	{
		 a1 = rand()%cores.size();
		 ts1 = a1 + cores.begin();
	}

	int a2 = rand()%cores.size();
	while(a1 == a2)
	{
		a2 = rand()%cores.size();
	}
	auto ts2 = a2 + cores.begin();
	
	//std::cout<<"ts1: "<<a1<<"   ts2: "<<a2<<std::endl;
	
	int i = rand()%(*ts1)->getTaskSetSize()+1;
	auto t = (*ts1)->getTask(i);
	moveTask(*ts1,*ts2,t);
	//std::cout<<"Allocation ends..."<<std::endl;
}

void Allocation::printAllocation(std::ostream& out)
{
	for(auto c = cores.begin();c!=cores.end();c++)
	{
		out<<"core: --------------"<<std::endl;
		(*c)->printTsExt(out);	
	}
}

bool Allocation::checkSchedulability()
{
	if(!updateOffsets())
		return false;
	for(auto ts : cores)
	{
		//if((*ts).getTaskSetSize() != 0)
		//{
			if(!ts->checkSchedulability())
			{
				//std::cout<<"not schedulable...."<<std::endl;
				return false;
			}
		//}
		//std::cout<<"taskset size = 0................."<<std::endl;
	}
	return true;
}

Task* Allocation::getTask(Function* f)
{
	for(auto ts:cores)
		for(auto t:ts->getTs())
			if(t->isMy(f))
				return t;
    return nullptr;
}

bool Allocation::cycleDetection()
{
	std::vector<Function* > runnables;

	for(auto ts: cores)
	{
		for(auto t: ts->getTs())
		{
			for(auto f: t->getFunctions())
				runnables.push_back(f);
		}
	}
	
	//std::pair<Task*, Task*> task_edge;
	std::vector<std::pair<TaskSet*, TaskSet*> > core_graph;
	for(auto r : runnables)
	{
		TaskSet* src = getTaskSet(r);
		for(auto succ : r->getSuccessors())
		{
			TaskSet* snk = getTaskSet(succ.first);
			if(src != snk )
			{
				std::pair<TaskSet*, TaskSet*> edge (src, snk);
				core_graph.push_back(edge);
			}
		}
	}
	
	//remove duplicated edges
	std::sort(core_graph.begin(),core_graph.end());
	core_graph.erase(std::unique(core_graph.begin(),core_graph.end()), core_graph.end());
	
	//for(auto cg:core_graph)
	//	std::cout<<cg.first<<" -> "<<cg.second<<std::endl;
	
	std::map<TaskSet*, int> visit;
	std::map<TaskSet*, vector<TaskSet*> > core_successors;
	
	for(auto ts:cores)
		visit[ts] = 0;
		
	for(auto cg:core_graph)
		core_successors[cg.first].push_back(cg.second);
	
	//DFS cycle detection
	for(auto u: cores)
	{
		if(visit[u] == 0)
		{
			if(cycleDetection_visit(core_successors,visit,u))
				return true;
		}
	}
	return false;
}

bool Allocation::cycleDetection_visit(std::map<TaskSet*,vector<TaskSet*> > succ, std::map<TaskSet*, int> &visit, TaskSet* u)
{
	visit[u] = 1;
	for(auto v:succ[u])
	{
		if(visit[v] == 0)
		{
			if(cycleDetection_visit(succ,visit,v))
				return true;
		}
		else if(visit[v] == 1)
			return true;
	}
	visit[u] = 2;
	return false;
}

std::vector<TaskSet* > Allocation::topologicalSortCores()
{
	std::vector<TaskSet*> stack;

	std::vector<Function* > runnables;

	for(auto ts: cores)
	{
		for(auto t: ts->getTs())
		{
			for(auto f: t->getFunctions())
				runnables.push_back(f);
		}
	}
	
	//std::pair<Task*, Task*> task_edge;
	std::vector<std::pair<TaskSet*, TaskSet*> > core_graph;
	for(auto r : runnables)
	{
		TaskSet* src = getTaskSet(r);
		for(auto succ : r->getSuccessors())
		{
			TaskSet* snk = getTaskSet(succ.first);
			if(src != snk )
			{
				std::pair<TaskSet*, TaskSet*> edge (src, snk);
				core_graph.push_back(edge);
			}
		}
	}
	
	//remove duplicated edges
	std::sort(core_graph.begin(),core_graph.end());
	core_graph.erase(std::unique(core_graph.begin(),core_graph.end()), core_graph.end());
	
	//for(auto cg:core_graph)
	//	std::cout<<cg.first<<" -> "<<cg.second<<std::endl;
	
	std::map<TaskSet*, int> visit;
	std::map<TaskSet*, vector<TaskSet*> > core_successors;
	
	for(auto ts:cores)
		visit[ts] = 0;
		
	for(auto cg:core_graph)
		core_successors[cg.first].push_back(cg.second);
	
	//DFS
	for(auto u: cores)
	{
		if(visit[u] == 0)
		{
			topo_visit(core_successors,visit,u,stack);
		}
	}
	vector<TaskSet*> order;
	for(int i=stack.size()-1;i>=0;i--)
	{
		order.push_back(stack[i]);
	}
	return order;
}

void Allocation::topo_visit(std::map<TaskSet*,vector<TaskSet*> > succ, std::map<TaskSet*, int> &visit, TaskSet* u, std::vector<TaskSet*>& stack)
{
	visit[u] = 1;
	for(auto v:succ[u])
	{
		if(visit[v] == 0)
		{
			topo_visit(succ,visit,v,stack);
		}
	}
	visit[u] = 2;
	stack.push_back(u);
}

bool Allocation::updateOffsets()
{
	clearOffsets();

	auto rtimes = getResponseTimes();//compute original rtimes for all functions
		
	auto offsets = setNewOffsets();//set new offsets based on rtimes
	
	auto rtimes_new = getResponseTimes();//compute new rtimes given new offsets
	
	int limits = 0;
	
	while(rtimes_new != rtimes && ++limits < 50)
	{
		rtimes = rtimes_new;
		
		offsets = setNewOffsets();
				
		rtimes_new = getResponseTimes();
	}
	if(rtimes_new != rtimes)
		return false;
		
	return true;

}



