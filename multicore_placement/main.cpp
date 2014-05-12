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

//  main.cpp

#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <time.h>
#include <string>

#include "Parser.h"
#include "TaskSet.h"
#include "RtwOpt.h"
#include "Allocation.h"
#include "sa_solver.h"

#define NI 1000
#define NC 4

#define ONLY_GS

#ifndef DEBUG
#include <omp.h>
#endif

int main( int argc, const char*   argv[] )
{

    time_t rawtime;
    time (&rawtime);
    std::cout << "Start at: " << ctime (&rawtime) << std::endl;
    
    // number of cores
    //std::string nc = argv[1];
    //int NC = (int)atof(nc.c_str());

    // Set SEED
#ifdef DEBUG
    srand(0);
#endif
#ifndef DEBUG
    srand(unsigned(time(NULL)*99999999999999));
#endif

    ofstream results_p;
    results_p.open("results.txt");


    std::vector<int> occurs_vector;
    for (int i = 10; i <= (NC*10); i++)
        occurs_vector.push_back(0);
    std::vector<int> memory_vector;
    for (int i = 10; i <= (NC*10); i++)
        memory_vector.push_back(0);
    
    std::vector<int> occurs_vector_sa;
    for (int i = 10; i <= (NC*10); i++)
        occurs_vector_sa.push_back(0);
    std::vector<int> memory_vector_sa;
    for (int i = 10; i <= (NC*10); i++)
        memory_vector_sa.push_back(0);
    
    std::vector<float> memory_relative;
    for (int i = 10; i <= (NC*10); i++)
        memory_relative.push_back(0);
    
    
    long counter = 0;
    
#ifndef DEBUG
#pragma omp parallel for
#endif
#ifndef DEBUG
    for (auto c = 10; c <= (NC*10); c++)
    {
#endif
#ifdef DEBUG
    for (auto c = 10; c <= (10); c++)
    {
#endif

        float load = c / 10.0;

#ifndef DEBUG
        
#pragma omp parallel for
        for (auto i = 0; i < NI; i++)
        {
#endif
            
#ifdef DEBUG
        for (auto i = 0; i < 1; i++)
        {
#endif
            
#ifndef DEBUG
#pragma omp critical(indexes)
#endif
            {
                ++counter;
                float done_perc = counter / (31.0*NI) * 100.0;
                std::cout << "Done: " << done_perc << " % " << std::endl;
                
                //std::cout << "Load: " << c << ", iteration: " << i << std::endl;
            }
            
            // Function graph generation
#ifndef DEBUG
            std::string tgff_file_name = "test_graph" + to_string(c) + "_" + to_string(i);
            std::string command = "./tgff " + tgff_file_name;
#endif
            
#ifdef DEBUG
            std::string tgff_file_name = "/Users/fabiocremona/Documents/SSSA-PhD/Works/FETA/multicore_placement/multicore_placement/test_graph" + to_string(c) + "_" + to_string(i);
            std::string command = "/Users/fabiocremona/Documents/SSSA-PhD/Works/FETA/multicore_placement/multicore_placement/tgff " + tgff_file_name;
#endif
            
            std::string command_rm = "rm " + tgff_file_name + '*';
            ofstream input;
            input.open(tgff_file_name + ".tgffopt");

            std::string graph = "tg_cnt 1\ntask_cnt 10 5\nprob_multi_start_nodes 1\nstart_node 2 1\ntg_write\neps_write";
            graph = "seed " + to_string(int(c*(i+1)*1000)) + "\n" + graph;

            input << graph;
            input.close();


            system(command.c_str());

            // Function FETA generation
            
            Parser ps0(tgff_file_name + ".tgff", load);
            ps0.create();
#ifdef DEBUG
            auto fun_list = ps0.getFunctions();
            std::cout << "Function list: " << std::endl;
            for (auto f : fun_list)
                f->print(std::cout);
            std::cout << std::endl << std::endl;
#endif
            // Remove tgff files
            system(command_rm.c_str());
            
            // Pseudo Gready Slacker
            
            Processor p0(ps0.getFunctions(), NC);
            bool is_sched_gs = p0.interCoreAllocation(load/NC);
            if (is_sched_gs == true)
            {
                
#ifndef DEBUG
#pragma omp atomic
#endif
                occurs_vector[c-10]++;
            }
            
            
            // Simulating Annealing
#ifndef ONLY_GS
            TaskSet ts0(*ps0.singleFunctionTask());
            Allocation alloc(&ts0, NC);
            Allocation alloc_final;
            alloc_final = sa_solver(alloc);
            alloc_final.computeRT();
            
            bool is_sched_sa = alloc_final.checkSchedulability();
            if (is_sched_sa == true)
            {
#pragma omp atomic
                occurs_vector_sa[c-10]++;
            }
            
            if (is_sched_gs && is_sched_sa)
            {
#pragma omp critical(dataupdate)
                {
                    memory_vector[c-10] += p0.getRT();
                    memory_vector_sa[c-10] += alloc_final.getRT();
                    float local_p = ( p0.getRT() - alloc_final.getRT() ) /
                                    ( p0.getRT() + alloc_final.getRT() ) * 100;
                    
                    memory_relative[c-10] = ( memory_relative[c-10] + local_p ) / 2.0;
                }
            }
#endif
        }
    }

#ifndef ONLY_GS
#pragma omp critical(output)
    {
        float load = 1.0;
        results_p << "Load\tGS\tSA\tM_GS\tM_SA" << std::endl;
        for (auto i = 0; i < occurs_vector.size(); i++)
        {
            results_p << load << "\t" << occurs_vector[i] << "\t"
            << occurs_vector_sa[i] << "\t" << memory_vector[i] << "\t"
            << memory_vector_sa[i] << std::endl;
            
            load += 0.1;
        }
    }
#endif

#ifdef ONLY_GS
        float load = 1.0;
        results_p << "Load\tM_GS" << std::endl;
        for (auto i = 0; i < occurs_vector.size(); i++)
        {
            results_p << load << "\t" << occurs_vector[i] << std::endl;
            
            load += 0.1;
        }
#endif

    results_p.close();

    time (&rawtime);
    std::cout << "End at: " << ctime (&rawtime) << std::endl;
    return 0;
}
