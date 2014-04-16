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
//  File: Block.h

#ifndef __tbd_framework__Block__
#define __tbd_framework__Block__

#include <iostream>

#include "Feta.h"




class Block : public Feta {

    float wcet;
public:
    
    Block();
    ~Block();
    
    void add(Feta*) {};
    void remove(Feta* ) {};
    float getWcet();
    
    void setUtilizationBound(float);
};
#endif /* defined(__tbd_framework__Block__) */
