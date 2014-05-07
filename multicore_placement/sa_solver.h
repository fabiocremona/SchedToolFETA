#ifndef _SIMULATED_ANNEALING_
#define _SIMULATED_ANNEALING_

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <limits.h>
#include <algorithm>
#include <math.h>
#include "Allocation.h"
#include "RtwOpt.h"

Allocation sa_solver(const Allocation& a);
//void initialize_sa(){};
void randomMove_sa(Allocation &a);
long computeCost_sa(Allocation a);

#endif