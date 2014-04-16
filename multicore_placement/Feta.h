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
// File: Feta.h

#ifndef __tbd_framework__Feta__
#define __tbd_framework__Feta__

#include <iostream>
#include <vector>

class Feta {
    
protected:
    long period;
    std::vector<float> places;
    
    /**
     Allows to add a new Feta object. The result is the sum of the two
     FETA objects.
     It is used from Function and Task to add blocks (functions) and
     functions (tasks)
     */
    void sum(Feta* );
    long offset;

public:
    Feta();
    Feta(float, long);
    Feta(std::vector<float>, long);
    ~Feta();
    
    // returns the distance (in time units) of the next firing location
    // starting from a given point in time (the input parameter)
    long dstNext(long);
    bool isFiring(long);
    float getFeta(long);
    long getPeriod();
    long getSize();
    
    const std::vector<float>& get();
    
    virtual void add(Feta *) = 0;
    virtual void remove(Feta *) = 0;
    
    void setOffset(float);
    long getOffset();
};




#endif /* defined(__tbd_framework__Feta__) */
