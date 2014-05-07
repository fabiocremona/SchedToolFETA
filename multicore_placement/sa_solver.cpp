#include "sa_solver.h"

using namespace std;

Allocation sa_solver(const Allocation& a)
{
	/*long offset = 0;
     do{
     offset = offset%a.getCoreNum();
     TaskSet* ts = a.getTaskSet(offset);
     (*ts).randomPartition();
     a.printAllocation(std::cout);
     offset++;
     }while(a.cycleDetection());*/
	Allocation alloc_best(a);
	
	Allocation alloc_tmp(a);
    
	Allocation alloc(a);
	
	double final_temp = 0.1;
	double temp = 100;
	double cool_factor = 0.99;
	double penalty_cost = computeCost_sa(a);
	double min_cost = penalty_cost*2;
	double current_cost = INT_MAX;
	int max_try = 4;
	
	//srand(time(NULL));
	srand(0);
	do{
		int cnt_try = 0;
		//alloc = alloc_tmp;
		while(cnt_try < max_try)
		{
			//std::cout<<"cnt_try "<<cnt_try<<std::endl;
			cnt_try++;
			//conduct random moves
			//(1) move function to tasks. (2) change priority (3) move task to cores
			randomMove_sa(alloc_tmp);
			//std::cout<<"schedulability: "<<alloc_tmp.checkSchedulability()<<" "<<temp<<std::endl;
			
			//compute current cost after random move
			current_cost = computeCost_sa(alloc_tmp);
			
			bool sched = alloc_tmp.checkSchedulability();
			//set penalty if the taskset is not schedulable
			if(!sched)//!alloc_tmp.checkSchedulability())
			{
				//std::cout<<"NOT SCHED "<<temp<<std::endl;
				//continue;
				current_cost = (current_cost+penalty_cost*2);
			}
			
			if (current_cost < min_cost)
			{
				min_cost = current_cost;
				alloc_best = alloc_tmp;
				if(sched)
					alloc = alloc_tmp;//alloc_tmp has to be schedulable
				//std::cout<<"[Accepted]\t"<<sched<<"\t"<<current_cost<<"\t"<<temp<<std::endl;
			}
			else
			{
				//if(current_cost == min_cost)
				//	continue;
				double randNum = rand()/double(RAND_MAX);
				double delta_cost = min_cost - current_cost;
				if(randNum < exp(0.8*delta_cost/temp))
				{
					min_cost = current_cost;
					alloc_best = alloc_tmp;
					//std::cout<<"[Cond Accepted]\t"<<sched<<"\t"<<current_cost<<"\t"<<temp<<std::endl;
				}
				else
				{
					//std::cout<<"[NOT accpeted]\t"<<sched<<"\t"<<current_cost<<"\t"<<temp<<std::endl;
				}
			}
		}
		temp *= cool_factor;
		alloc_tmp = alloc_best;
		
	}while(temp > final_temp);
	
	//cout<<"simulated annealing mcmr"<<endl;
	return alloc;
}

void randomMove_sa(Allocation &a)
{
	//srand(time(NULL));
	int type = rand()%4;
	long offset;
	TaskSet* ts;
	if(type != 3)
	{
		do{
			offset = rand()%a.getCoreNum();
			ts = a.getTaskSet(offset);
		}while((*ts).getTaskSetSize() == 0);
	}
	switch(type){
		case 0:
			//std::cout<<"Swap...."<<std::endl;
			(*ts).randomSwapPriorities();
			break;
			
		case 1:
			//std::cout<<"Merge...."<<std::endl;
			(*ts).randomMerge();
			break;
			
		case 2:
			//std::cout<<"Partition..."<<std::endl;
			(*ts).randomPartition();
			break;
			
		case 3:
			//std::cout<<"Allocation..."<<std::endl;
			a.randomMoveAllocation();
			//int cnt=0;
			//Allocation a_tmp(a);
			//do{
			//	a_tmp.randomMoveAllocation();
			//}while(a_tmp.cycleDetection() && cnt < 10);
			//if(cnt < 10)
			//	a = a_tmp;
			break;
	}
}

long computeCost_sa(Allocation a)
{
	a.computeRT();
	
	//std::cout<<"current cost: "<<a.getRT()<<std::endl;
	
	return a.getRT();
	/*long mem = 0;
     for(int i=0;i<a.getCoreNum();i++)
     {
     auto ts = a.getTaskSet(i);
     RtwOpt opt(*ts);
     mem += opt.computeRTB2();
     }
     return mem;*/
}

