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
//  File: Block.cpp

#include "Block.h"
#include "Extras.h"
#include <algorithm>



Block::Block() : Feta()
{
    std::vector<int> base_periods;
    base_periods.push_back(1);
    base_periods.push_back(2);
    base_periods.push_back(3);
    base_periods.push_back(4);
    base_periods.push_back(5);
    base_periods.push_back(6);
    
    /**
     Generate the period randomly, by choosing it between one of the values
     in base_periods
     */
    //float M = 4 * (float)rand() / (float)RAND_MAX;
    int N = 1 + int(3 * ((float)rand() / (float)RAND_MAX));
    period = 1;
    for (int i = 0; i < N; i++)
    {
        int idx = (int)(base_periods.size() * (float)rand() / (float)RAND_MAX);
        period = period * base_periods[idx];
    }
    
    /**
     Generate the block WCET randomly, by choosing a number between [0, 1]
     */
    wcet = (float)rand()/(float)RAND_MAX;
    places.push_back( wcet );

}


Block::~Block()
{

}

float Block::getWcet()
{
    return wcet;
}

void Block::setUtilizationBound(float ub)
{
    
}
