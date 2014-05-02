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
#include <omp.h>


#include "Parser.h"
#include "TaskSet.h"
#include "RtwOpt.h"
#include "Processor.h"

#include <stdio.h>
#include <stdlib.h>

#define MULTIPLIER 99999999999999

// Arguments:
// #cores, Ub

int main( int argc, const char*   argv[] )
{
    
    // number of cores
    int NI = 100;
    
    std::string nc = argv[1];
    int NC = (int)atof(nc.c_str());
    
    std::string ub = argv[2];
    int UB = (int)atof(ub.c_str());
    
    ofstream output;
    output.open("results.txt");
    
    // enable random seed
    srand(unsigned(time(NULL)*99999999999999));
    std::map<float, float> results;
    int occurs = 0;
    
    for (auto u = 10; u <= (NC*10); u += 1)
    {
        occurs = 0;
        
        #pragma omp parallel for
        for (auto i = 0; i < NI; i++)
        {
            float U = u / 10;
            std::string tgff_file_name = "test_graph" + to_string(int(u)) + "_" + to_string(i);
            ofstream input;
            input.open(tgff_file_name + ".tgffopt");
            
            std::string graph = "tg_cnt 1\ntask_cnt 10 5\nprob_multi_start_nodes 1\nstart_node 2 1\ntg_write\neps_write";
            graph = "seed " + to_string(int(u*i*1000)) + "\n" + graph;
            
            input << graph;
            input.close();
            
            #pragma omp critical
            {
            std::string command = "./tgff " + tgff_file_name;
            system(command.c_str());
            }
            
            Parser ps0(tgff_file_name + ".tgff", to_string(U));
            ps0.create();
            
            #pragma omp critical
            {
                output << "Load: " << U << " - Iteration: " << i << std::endl;
                output << "Functions: " << std::endl;
                for (auto f : ps0.getFunctions())
                    f->print(output);
                std::cout << "Load: " << U << " - Iteration: " << i << std::endl;
            }
            
            Processor p0(ps0.getFunctions(), NC);
            bool is_sched = p0.interCoreAllocation(UB);
            
            if (is_sched == true)
            {
                #pragma omp critical
                {
                    output << "Solution schedulable" << std::endl;
                    p0.print(output);
                    occurs++;
                }
            }
            else
            {
                #pragma omp critical
                {
                    output << "Solution NOT schedulable" << std::endl;
                }
            }
        }
        #pragma omp critical
        {
            results[u] = occurs/NI*100;
            output << "===> Schedulability: " << occurs/NI*100 << "%" << std::endl;
            std::cout << "===> Schedulability: " << occurs/NI*100 << "%" << std::endl;
        }
    }
    
    ofstream results_p;
    results_p.open("results_percentage.txt");
    for (auto r : results)
        results_p << r.first << "\t" << r.second << std::endl;
    results_p.close();
    
    
    output.close();
    return 0;
}



