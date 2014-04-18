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

//  File: Feta.cpp

#include "Feta.h"
#include "Extras.h"
#include "Block.h"
#include <algorithm>
#include <math.h>

Feta::Feta()
{
    offset = 0;
}

Feta::Feta(float c, long p)
{
    offset = 0;
    places.push_back(c);
    period = p;
}

Feta::Feta(std::vector<float> p, long t)
{
    offset = 0;
    places = p;
    period = t;
}

Feta::~Feta()
{
    
}

bool Feta::isFiring(long t)
{
    if ( (t - offset) % period == 0 )
        return true;
    else
        return false;
}

float Feta::getFeta(long t)
{
    if (isFiring(t))
    {
        long hyperperiod = places.size() * period;
        if (t < offset)
            return float(0);
        else
            return places[long(t / period) % long(hyperperiod / period)];
    }
    return float(0);
}

long Feta::dstNext(long t)
{
    if (offset > 0)
    {
        std::cout << "Error: using dstNext with a task with offset" << std::endl;
        exit(-1);
    }
    long hyperperiod = places.size() * period;
    int dst = 0;
    for (auto i = (t % hyperperiod) + 1; i <= places.size(); i++)
    {
        dst++;
        if (isFiring(i * period) && getFeta(i * period))
            return (dst * period);
        
    }    
    return (++dst) * period;
}

long Feta::getPeriod()
{
    return period;
}

long Feta::getSize()
{
    return places.size();
}

void Feta::sum(Feta *feta)
{
    // summ can not be used with Blocks type!!!
    if (!dynamic_cast<Block*>(this))
    {
        if (places.size() == 0)
        {
            places = feta->get();
            period = feta->getPeriod();
        }
        else
        {
            // Compute the period
            period = gcd(period, feta->getPeriod());
            // Compute the current hiperperiod
            long hyp_p = feta->getPeriod() * feta->get().size();
            
            // Compute the future hyperperiod
            long future_hyp_p = gcd(hyp_p, feta->getSize() * feta->getPeriod());
            
            long new_size = future_hyp_p / period;
            
            // Create the new FETA
            std::vector<float> new_feta;
            for (auto i = 0 ; i < new_size; ++i) new_feta.push_back(0);
            
            // Compute the new FETA
            auto t = 0;
            
            for (auto i : new_feta)
            {
                i = getFeta(t) + feta->getFeta(t);
                t += period;
            }
            
            places.clear();
            places = new_feta;
        }
    }
}

const std::vector<float>& Feta::get()
{
    return places;
}

void Feta::setOffset(float o)
{
    offset = (long)ceil(o);
}

long Feta::getOffset()
{
    return offset;
}


