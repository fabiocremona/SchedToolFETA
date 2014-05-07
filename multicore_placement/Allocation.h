#ifndef __feta_Allocation__
#define __feta_Allocation__

#include <iostream>
#include <vector>
#include <algorithm>
#include "Task.h"
#include "TaskSet.h"

using namespace std;

class Allocation{
	//int num_cores;
	std::vector<TaskSet*> cores;
	
	long total_size;
	long rt2_size;
	long rt1_size;

public:
	Allocation();
	Allocation(TaskSet*, int);
	~Allocation();
	Allocation(const Allocation& other);
	Allocation& operator = (const Allocation& other);
	
	std::vector<TaskSet*>& getCores(); 
	TaskSet* getTaskSet(long);//find taskset by offset
	TaskSet* getTaskSet(Function*);//find taskset by function
	Task* getTask(Function*);//find task by function
	
	// Compute the toal amount of memory required by RT blocks (type 1 and 2)
    // for inter core
    void computeRT();
    
    // get the total amount of memory required by RT blocks of type 1
    long getRT1();
    
    // get the total amount of memory required by RT blocks of type 2
    long getRT2();
    
    // fet the total amount of memory required by RT blocks
    long getRT();
	
	//get response times for all runnables
	std::map<Function*, float> getResponseTimes();
	
	//get offsets for all tasks
	std::map<Task*, long> getOffsets();
	
	// Compute the new offsets for all tasks
    std::map<Task*, float> getNewOffsets();
	
	// Set the new offsets
    std::map<Task*, float> setNewOffsets();   
    void setOffsets(std::map<Task*, float>);
	
	//clear offsets
	void clearOffsets();
	
	int getCoreNum();
	void moveTask(TaskSet* src, TaskSet* dst, Task* t);
	void randomMoveAllocation();//randomly select two task sets, and randomly select one task with random priorities.
	void printAllocation(ostream&);
	bool checkSchedulability();
	
	//update offset for each tasks for current allocation
	bool updateOffsets();//update task offsets
	
	//detect cycles on core level
	bool cycleDetection();
	bool cycleDetection_visit(std::map<TaskSet*, vector<TaskSet*> > succ, std::map<TaskSet*, int> &visit, TaskSet* u);
	
	//topological sort on core level.
	std::vector<TaskSet*> topologicalSortCores();//return topo order.
	void topo_visit(std::map<TaskSet*, vector<TaskSet*> > succ, std::map<TaskSet*, int> &visit, TaskSet* ts, std::vector<TaskSet* >& stack);
	
};

#endif /*(defined __feta_Allocation__)*/