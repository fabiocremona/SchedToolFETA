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
//  File: Component.h

#ifndef __tbd_framework__Component__
#define __tbd_framework__Component__

#include <iostream>
#include <string>
#include <vector>

#include "Function.h"

class Component {
    
    std::string name;
    std::map<std::string, Function *> functions;
    
public:
    
    Component(std::string);
    ~Component();
    
    void addFunction(Function *);
    void connect(Function *, Function *);
    void connect(Function *, Function *, long);
    void setUtilizationBound(float);
    double get_uc();
    std::map<std::string, Function *>& getFunctions();
};


#endif /* defined(__tbd_framework__Component__) */
