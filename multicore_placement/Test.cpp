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

//  Test.cpp

#include <iostream>
#include <fstream>
#include <algorithm>

#include "Parser.h"
#include "TaskSet.h"
#include "RtwOpt.h"
#include "Processor.h"

#define CATCH_CONFIG_MAIN
#include "catch.h"


//#include "taskset_test.h"
//#include "feta_test.h"
//#include "task_test.h"


//#include "other_tests.h"


TEST_CASE("getFeta() no offset", "[feta]")
{
    std::vector<float> v0;
    v0.push_back(1); v0.push_back(0); v0.push_back(2); v0.push_back(0); v0.push_back(1); v0.push_back(0);v0.push_back(0); v0.push_back(0); v0.push_back(1); v0.push_back(0); v0.push_back(0); v0.push_back(0); v0.push_back(0); v0.push_back(0); v0.push_back(2); v0.push_back(0);
    
    Function f0("f0", 3, v0);
    Task t0;
    t0.add(&f0);
    TaskSet ts0;
    ts0.add(&t0);
    
    REQUIRE(t0.getFeta(0) == 1);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 0);
    REQUIRE(t0.getFeta(4) == 0);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 2);
    REQUIRE(t0.getFeta(7) == 0);
    REQUIRE(t0.getFeta(8) == 0);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 0);
    REQUIRE(t0.getFeta(12) == 1);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 0);
    REQUIRE(t0.getFeta(16) == 0);
    REQUIRE(t0.getFeta(17) == 0);
    REQUIRE(t0.getFeta(18) == 0);
    REQUIRE(t0.getFeta(19) == 0);
    REQUIRE(t0.getFeta(20) == 0);
    REQUIRE(t0.getFeta(21) == 0);
    REQUIRE(t0.getFeta(22) == 0);
    REQUIRE(t0.getFeta(23) == 0);
    REQUIRE(t0.getFeta(24) == 1);
    REQUIRE(t0.getFeta(25) == 0);
    REQUIRE(t0.getFeta(26) == 0);
    REQUIRE(t0.getFeta(27) == 0);
    REQUIRE(t0.getFeta(28) == 0);
    REQUIRE(t0.getFeta(29) == 0);
    REQUIRE(t0.getFeta(30) == 0);
    REQUIRE(t0.getFeta(31) == 0);
    REQUIRE(t0.getFeta(32) == 0);
    REQUIRE(t0.getFeta(33) == 0);
    REQUIRE(t0.getFeta(34) == 0);
    REQUIRE(t0.getFeta(35) == 0);
    REQUIRE(t0.getFeta(36) == 0);
    REQUIRE(t0.getFeta(37) == 0);
    REQUIRE(t0.getFeta(38) == 0);
    REQUIRE(t0.getFeta(39) == 0);
    REQUIRE(t0.getFeta(40) == 0);
    REQUIRE(t0.getFeta(41) == 0);
    REQUIRE(t0.getFeta(42) == 2);
    REQUIRE(t0.getFeta(43) == 0);
    REQUIRE(t0.getFeta(44) == 0);
    REQUIRE(t0.getFeta(45) == 0);
    REQUIRE(t0.getFeta(46) == 0);
    REQUIRE(t0.getFeta(47) == 0);
    REQUIRE(t0.getFeta(48) == 1);
    REQUIRE(t0.getFeta(49) == 0);
}

TEST_CASE("getFeta() with offset", "[feta]")
{
    std::vector<float> v0;
    v0.push_back(1);
    v0.push_back(0);
    v0.push_back(2);
    v0.push_back(0);
    v0.push_back(1);
    v0.push_back(0);
    v0.push_back(0);
    v0.push_back(0);
    v0.push_back(1);
    v0.push_back(0);
    v0.push_back(0);
    v0.push_back(0);
    v0.push_back(0);
    v0.push_back(0);
    v0.push_back(2);
    v0.push_back(0);
    
    Function f0("f0", 3, v0);
    Task t0;
    t0.setOffset(6);
    t0.add(&f0);
    
    REQUIRE(t0.getFeta(0) == 0);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 0);
    REQUIRE(t0.getFeta(4) == 0);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 1);
    REQUIRE(t0.getFeta(7) == 0);
    REQUIRE(t0.getFeta(8) == 0);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 0);
    REQUIRE(t0.getFeta(12) == 2);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 0);
    REQUIRE(t0.getFeta(16) == 0);
    REQUIRE(t0.getFeta(17) == 0);
    REQUIRE(t0.getFeta(18) == 1);
    REQUIRE(t0.getFeta(19) == 0);
    REQUIRE(t0.getFeta(20) == 0);
    REQUIRE(t0.getFeta(21) == 0);
    REQUIRE(t0.getFeta(22) == 0);
    REQUIRE(t0.getFeta(23) == 0);
    REQUIRE(t0.getFeta(24) == 0);
    REQUIRE(t0.getFeta(25) == 0);
    REQUIRE(t0.getFeta(26) == 0);
    REQUIRE(t0.getFeta(27) == 0);
    REQUIRE(t0.getFeta(28) == 0);
    REQUIRE(t0.getFeta(29) == 0);
    REQUIRE(t0.getFeta(30) == 1);
    REQUIRE(t0.getFeta(31) == 0);
    REQUIRE(t0.getFeta(32) == 0);
    REQUIRE(t0.getFeta(33) == 0);
    REQUIRE(t0.getFeta(34) == 0);
    REQUIRE(t0.getFeta(35) == 0);
    REQUIRE(t0.getFeta(36) == 0);
    REQUIRE(t0.getFeta(37) == 0);
    REQUIRE(t0.getFeta(38) == 0);
    REQUIRE(t0.getFeta(39) == 0);
    REQUIRE(t0.getFeta(40) == 0);
    REQUIRE(t0.getFeta(41) == 0);
    REQUIRE(t0.getFeta(42) == 0);
    REQUIRE(t0.getFeta(43) == 0);
    REQUIRE(t0.getFeta(44) == 0);
    REQUIRE(t0.getFeta(45) == 0);
    REQUIRE(t0.getFeta(46) == 0);
    REQUIRE(t0.getFeta(47) == 0);
    REQUIRE(t0.getFeta(48) == 2);
    REQUIRE(t0.getFeta(49) == 0);
    REQUIRE(t0.getFeta(50) == 0);
    REQUIRE(t0.getFeta(51) == 0);
    REQUIRE(t0.getFeta(52) == 0);
    REQUIRE(t0.getFeta(53) == 0);
    REQUIRE(t0.getFeta(54) == 1);
    REQUIRE(t0.getFeta(55) == 0);
    REQUIRE(t0.getFeta(56) == 0);
    REQUIRE(t0.getFeta(57) == 0);
    REQUIRE(t0.getFeta(58) == 0);
    REQUIRE(t0.getFeta(59) == 0);
    REQUIRE(t0.getFeta(60) == 2);
}

TEST_CASE("getDst(long)", "[feta]")
{
    std::vector<float> v0;
    v0.push_back(1); v0.push_back(0); v0.push_back(2); v0.push_back(0); v0.push_back(1); v0.push_back(0);v0.push_back(0); v0.push_back(0); v0.push_back(1); v0.push_back(0); v0.push_back(0); v0.push_back(0); v0.push_back(0); v0.push_back(0); v0.push_back(2); v0.push_back(0);
    
    Function f0("f0", 3, v0);
    Task t0;
    t0.setOffset(6);
    t0.add(&f0);
    TaskSet ts0;
    ts0.add(&t0);
    REQUIRE(t0.dstNext(0) == 6);
    REQUIRE(t0.dstNext(1) == 5);
    REQUIRE(t0.dstNext(2) == 4);
    REQUIRE(t0.dstNext(3) == 3);
    REQUIRE(t0.dstNext(4) == 2);
    REQUIRE(t0.dstNext(5) == 1);
    REQUIRE(t0.dstNext(6) == 6);
    REQUIRE(t0.dstNext(7) == 5);
    REQUIRE(t0.dstNext(8) == 4);
    REQUIRE(t0.dstNext(9) == 3);
    REQUIRE(t0.dstNext(10) == 2);
    REQUIRE(t0.dstNext(11) == 1);
    REQUIRE(t0.dstNext(12) == 6);
    REQUIRE(t0.dstNext(13) == 5);
    REQUIRE(t0.dstNext(14) == 4);
    REQUIRE(t0.dstNext(15) == 3);
    REQUIRE(t0.dstNext(16) == 2);
    REQUIRE(t0.dstNext(17) == 1);
    REQUIRE(t0.dstNext(18) == 12);
    REQUIRE(t0.dstNext(19) == 11);
}

TEST_CASE( "Task::add(Feta*), no offset", "[task]" )
{
    
    std::vector<float> v0;
    std::vector<float> v1;
    v0.push_back(2); v0.push_back(0); v0.push_back(1); v0.push_back(0);
    v1.push_back(5); v1.push_back(0); v1.push_back(3); v1.push_back(0);
    
    Function f0("f0", 2, v0);
    Function f1("f1", 2, v1);
    
    Task t0;
    t0.add(&f0);
    t0.add(&f1);
    
    REQUIRE(t0.getFunSize() == 2);
    REQUIRE(t0.getFeta(0) == 7);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 0);
    REQUIRE(t0.getFeta(4) == 4);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 0);
    REQUIRE(t0.getFeta(8) == 7);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 0);
    REQUIRE(t0.getFeta(12) == 4);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 0);
    REQUIRE(t0.getFeta(16) == 7);
    
    t0.remove(&f0);
    
    REQUIRE(t0.getFunSize() == 1);
    REQUIRE(t0.getFeta(0) == 5);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 0);
    REQUIRE(t0.getFeta(4) == 3);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 0);
    REQUIRE(t0.getFeta(8) == 5);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 0);
    REQUIRE(t0.getFeta(12) == 3);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 0);
    REQUIRE(t0.getFeta(16) == 5);
    
    t0.add(&f0);
    t0.remove(&f1);
    
    REQUIRE(t0.getFunSize() == 1);
    REQUIRE(t0.getFeta(0) == 2);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 0);
    REQUIRE(t0.getFeta(4) == 1);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 0);
    REQUIRE(t0.getFeta(8) == 2);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 0);
    REQUIRE(t0.getFeta(12) == 1);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 0);
    REQUIRE(t0.getFeta(16) == 2);
}

TEST_CASE( "Task::add(Feta*), offset - one", "[task]" )
{
    
    std::vector<float> v0;
    std::vector<float> v1;
    v0.push_back(2); v0.push_back(0); v0.push_back(1); v0.push_back(0);
    v1.push_back(5); v1.push_back(0); v1.push_back(3); v1.push_back(0);
    
    Function f0("f0", 2, v0);
    Function f1("f1", 2, v1);
    
    Task t0;
    t0.add(&f0);
    t0.add(&f1);
    t0.setOffset(3);
    
    REQUIRE(t0.getFunSize() == 2);
    
    REQUIRE(t0.getFeta(0) == 0);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 7);
    REQUIRE(t0.getFeta(4) == 0);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 4);
    REQUIRE(t0.getFeta(8) == 0);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 7);
    REQUIRE(t0.getFeta(12) == 0);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 4);
    REQUIRE(t0.getFeta(16) == 0);
    REQUIRE(t0.getFeta(17) == 0);
    REQUIRE(t0.getFeta(18) == 0);
    REQUIRE(t0.getFeta(19) == 7);
    
    t0.remove(&f0);
    
    REQUIRE(t0.getFunSize() == 1);
    REQUIRE(t0.getFeta(0) == 0);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 5);
    REQUIRE(t0.getFeta(4) == 0);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 3);
    REQUIRE(t0.getFeta(8) == 0);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 5);
    REQUIRE(t0.getFeta(12) == 0);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 3);
    REQUIRE(t0.getFeta(16) == 0);
    REQUIRE(t0.getFeta(17) == 0);
    REQUIRE(t0.getFeta(18) == 0);
    REQUIRE(t0.getFeta(19) == 5);
    
    t0.add(&f0);
    t0.remove(&f1);
    
    REQUIRE(t0.getFeta(0) == 0);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 2);
    REQUIRE(t0.getFeta(4) == 0);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 1);
    REQUIRE(t0.getFeta(8) == 0);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 2);
    REQUIRE(t0.getFeta(12) == 0);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 1);
    REQUIRE(t0.getFeta(16) == 0);
    REQUIRE(t0.getFeta(17) == 0);
    REQUIRE(t0.getFeta(18) == 0);
    REQUIRE(t0.getFeta(19) == 2);
}

TEST_CASE( "Task::add(Feta*), offset - two", "[task]" )
{
    
    std::vector<float> v0;
    std::vector<float> v1;
    v0.push_back(2); v0.push_back(0); v0.push_back(1); v0.push_back(0);
    v1.push_back(5); v1.push_back(0); v1.push_back(3); v1.push_back(0);
    
    Function f0("f0", 2, v0);
    Function f1("f1", 3, v1);
    
    Task t0;
    t0.add(&f0);
    t0.add(&f1);
    t0.setOffset(3);
    
    REQUIRE(t0.getFunSize() == 2);
    
    REQUIRE(t0.getFeta(0) == 0);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 7);
    REQUIRE(t0.getFeta(4) == 0);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 1);
    REQUIRE(t0.getFeta(8) == 0);
    REQUIRE(t0.getFeta(9) == 3);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 2);
    REQUIRE(t0.getFeta(12) == 0);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 6);
    REQUIRE(t0.getFeta(16) == 0);
    REQUIRE(t0.getFeta(17) == 0);
    REQUIRE(t0.getFeta(18) == 0);
    REQUIRE(t0.getFeta(19) == 2);
    REQUIRE(t0.getFeta(20) == 0);
    REQUIRE(t0.getFeta(21) == 3);
    REQUIRE(t0.getFeta(22) == 0);
    REQUIRE(t0.getFeta(23) == 1);
    REQUIRE(t0.getFeta(24) == 0);
    
    t0.remove(&f0);
    
    REQUIRE(t0.getFunSize() == 1);
    REQUIRE(t0.getFeta(0) == 0);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 5);
    REQUIRE(t0.getFeta(4) == 0);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 0);
    REQUIRE(t0.getFeta(8) == 0);
    REQUIRE(t0.getFeta(9) == 3);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 0);
    REQUIRE(t0.getFeta(12) == 0);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 5);
    REQUIRE(t0.getFeta(16) == 0);
    REQUIRE(t0.getFeta(17) == 0);
    REQUIRE(t0.getFeta(18) == 0);
    REQUIRE(t0.getFeta(19) == 0);
    REQUIRE(t0.getFeta(20) == 0);
    REQUIRE(t0.getFeta(21) == 3);
    REQUIRE(t0.getFeta(22) == 0);
    REQUIRE(t0.getFeta(23) == 0);
    REQUIRE(t0.getFeta(24) == 0);
    
    t0.add(&f0);
    t0.remove(&f1);
    
    REQUIRE(t0.getFeta(0) == 0);
    REQUIRE(t0.getFeta(1) == 0);
    REQUIRE(t0.getFeta(2) == 0);
    REQUIRE(t0.getFeta(3) == 2);
    REQUIRE(t0.getFeta(4) == 0);
    REQUIRE(t0.getFeta(5) == 0);
    REQUIRE(t0.getFeta(6) == 0);
    REQUIRE(t0.getFeta(7) == 1);
    REQUIRE(t0.getFeta(8) == 0);
    REQUIRE(t0.getFeta(9) == 0);
    REQUIRE(t0.getFeta(10) == 0);
    REQUIRE(t0.getFeta(11) == 2);
    REQUIRE(t0.getFeta(12) == 0);
    REQUIRE(t0.getFeta(13) == 0);
    REQUIRE(t0.getFeta(14) == 0);
    REQUIRE(t0.getFeta(15) == 1);
    REQUIRE(t0.getFeta(16) == 0);
    REQUIRE(t0.getFeta(17) == 0);
    REQUIRE(t0.getFeta(18) == 0);
    REQUIRE(t0.getFeta(19) == 2);
    REQUIRE(t0.getFeta(20) == 0);
    REQUIRE(t0.getFeta(21) == 0);
    REQUIRE(t0.getFeta(22) == 0);
    REQUIRE(t0.getFeta(23) == 1);
    REQUIRE(t0.getFeta(24) == 0);
}

TEST_CASE( "TaskSet::schedulability() - one", "[taskset]" )
{
    std::vector<float> v0;
    std::vector<float> v1;
    v0.push_back(1); v0.push_back(0); v0.push_back(1); v0.push_back(0);
    v1.push_back(1); v1.push_back(0); v1.push_back(1); v1.push_back(0);
    
    Function f0("f0", 2, v0);
    Function f1("f1", 1, v1);
    Task t0; Task t1;
    t0.add(&f0); t1.add(&f1);
    TaskSet ts0;
    
    t0.changePriority(2);
    t1.changePriority(1);
    
    ts0.add(&t0);
    ts0.add(&t1);
    ts0.computeFeta();
    REQUIRE(ts0.getTs().size() == 2);
    REQUIRE(ts0.getPeriod() == 1);
    REQUIRE(ts0.getSize() == 8);
    
    ts0.remove(&t1);
    REQUIRE(ts0.checkSchedulability() == true);
    
    ts0.remove(&t0);
    ts0.add(&t1);
    ts0.computeFeta();
    REQUIRE(ts0.getTs().size() == 1);
    REQUIRE(ts0.getPeriod() == 1);
    REQUIRE(ts0.getSize() == 4);
    REQUIRE(ts0.checkSchedulability() == true);
    
    ts0.add(&t0);
    REQUIRE(ts0.checkSchedulability() == true);
}

TEST_CASE( "TaskSet::checkSchedulability() - two", "[taskset]" )
{
    std::vector<float> v0;
    std::vector<float> v1;
    v0.push_back(1); v0.push_back(1); v0.push_back(1); v0.push_back(0);
    v1.push_back(1); v1.push_back(0.5); v1.push_back(1); v1.push_back(0.5);
    
    Function f0("f0", 2, v0);
    Function f1("f1", 1, v1);
    Task t0; Task t1;
    t0.add(&f0); t1.add(&f1);
    TaskSet ts0;
    
    t0.changePriority(2);
    t1.changePriority(1);
    
    ts0.add(&t0);
    ts0.add(&t1);
    
    REQUIRE(ts0.checkSchedulability() == false);
}

TEST_CASE( "TaskSet::computeRbf() - TaskSet::checkSchedulability() - one", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(0.2);
    std::vector<float> f1; f1.push_back(0.2);
    std::vector<float> f2; f2.push_back(0.2);
    std::vector<float> f3; f3.push_back(0.2);
    std::vector<float> f4; f4.push_back(0.2);
    std::vector<float> f5; f5.push_back(0.2);
    
    Function ff0("ff0", 2, f0); Function ff1("ff1", 2, f1); Function ff2("ff2", 2, f2);
    Function ff3("ff3", 1, f3); Function ff4("ff4", 1, f4); Function ff5("ff5", 1, f5);
    
    std::vector<Function*> fs0; std::vector<Function*> fs1;
    fs0.push_back(&ff0); fs0.push_back(&ff1); fs0.push_back(&ff2);
    fs1.push_back(&ff3); fs1.push_back(&ff4); fs1.push_back(&ff5);
    
    Task t0; Task t1;
    t0.add(&fs0);
    t1.add(&fs1);
    
    t0.changePriority(2);
    t1.changePriority(1);
    TaskSet ts0;
    ts0.add(&t0); ts0.add(&t1);
    
    REQUIRE(ts0.checkSchedulability() == true);
    REQUIRE(t0.getPeriod() == 2);
    REQUIRE(t1.getPeriod() == 1);
    
    REQUIRE(ts0.getRbf(0) == Approx(1.2));
    REQUIRE(ts0.getRbf(1) == Approx(1.8));
    REQUIRE(ts0.getRbf(2) == Approx(3));
    REQUIRE(ts0.getRbf(3) == Approx(3.6));
    REQUIRE(ts0.getRbf(4) == Approx(4.8));
    REQUIRE(ts0.getRbf(5) == Approx(5.4));
    REQUIRE(ts0.getRbf(6) == Approx(6.6));
    REQUIRE(ts0.getRbf(7) == Approx(7.2));
    REQUIRE(ts0.getRbf(8) == Approx(8.4));
    REQUIRE(ts0.getRbf(9) == Approx(9));
    REQUIRE(ts0.getRbf(10) == Approx(10.2));
    REQUIRE(ts0.getRbf(11) == Approx(10.8));
    REQUIRE(ts0.getRbf(12) == Approx(12));
}

TEST_CASE( "TaskSet::computeRbf() - TaskSet::checkSchedulability() - two (with offset)", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(0.2); f0.push_back(0.3); f0.push_back(0.4);
    Function ff0("ff0", 2, f0);
    Function ff1("ff1", 2, f0);
    
    std::vector<Function*> fs0;
    fs0.push_back(&ff0);
    
    Task t0; Task t1;
    t0.add(&ff0);
    t1.add(&ff1);
    t1.setOffset(6);
    TaskSet ts0;
    ts0.add(&t0); ts0.add(&t1);
    t0.changePriority(1);
    t1.changePriority(2);
    
    REQUIRE(ts0.checkSchedulability() == true);
    REQUIRE(t1.getFeta(0) == Approx(0));
    REQUIRE(t1.getFeta(2) == Approx(0));
    REQUIRE(t1.getFeta(4) == Approx(0));
    REQUIRE(t1.getFeta(6) == Approx(0.2));
    REQUIRE(t1.getFeta(8) == Approx(0.3));
    REQUIRE(t1.getFeta(10) == Approx(0.4));
    REQUIRE(t1.getFeta(12) == Approx(0.2));
    REQUIRE(t1.getFeta(14) == Approx(0.3));
    REQUIRE(t1.getFeta(16) == Approx(0.4));
    REQUIRE(t1.getFeta(18) == Approx(0.2));
    REQUIRE(t1.getFeta(20) == Approx(0.3));
    REQUIRE(t1.getFeta(22) == Approx(0.4));
    
    REQUIRE(t0.getFeta(0) == Approx(0.2));
    REQUIRE(t0.getFeta(2) == Approx(0.3));
    REQUIRE(t0.getFeta(4) == Approx(0.4));
    REQUIRE(t0.getFeta(6) == Approx(0.2));
    REQUIRE(t0.getFeta(8) == Approx(0.3));
    REQUIRE(t0.getFeta(10) == Approx(0.4));
    REQUIRE(t0.getFeta(12) == Approx(0.2));
    REQUIRE(t0.getFeta(14) == Approx(0.3));
    REQUIRE(t0.getFeta(16) == Approx(0.4));
    REQUIRE(t0.getFeta(18) == Approx(0.2));
    REQUIRE(t0.getFeta(20) == Approx(0.3));
    REQUIRE(t0.getFeta(22) == Approx(0.4));
    
    REQUIRE(ts0.getRbf(0) == Approx(0.2));
    REQUIRE(ts0.getRbf(1) == Approx(0.2));
    REQUIRE(ts0.getRbf(2) == Approx(0.5));
    REQUIRE(ts0.getRbf(3) == Approx(0.5));
    REQUIRE(ts0.getRbf(4) == Approx(0.9));
    REQUIRE(ts0.getRbf(5) == Approx(0.9));
    REQUIRE(ts0.getRbf(6) == Approx(1.3));
    REQUIRE(ts0.getRbf(7) == Approx(1.3));
    REQUIRE(ts0.getRbf(8) == Approx(1.9));
    REQUIRE(ts0.getRbf(9) == Approx(1.9));
    REQUIRE(ts0.getRbf(10) == Approx(2.7));
    REQUIRE(ts0.getRbf(11) == Approx(2.7));
    REQUIRE(ts0.getRbf(12) == Approx(3.1));
    REQUIRE(ts0.getRbf(13) == Approx(3.1));
    REQUIRE(ts0.getRbf(14) == Approx(3.7));
    REQUIRE(ts0.getRbf(15) == Approx(3.7));
    REQUIRE(ts0.getRbf(16) == Approx(4.5));
    REQUIRE(ts0.getRbf(17) == Approx(4.5));
    REQUIRE(ts0.getRbf(18) == Approx(4.9));
    REQUIRE(ts0.getRbf(19) == Approx(4.9));
    REQUIRE(ts0.getRbf(20) == Approx(5.5));
    REQUIRE(ts0.getRbf(21) == Approx(5.5));
    REQUIRE(ts0.getRbf(22) == Approx(6.3));
    REQUIRE(ts0.getRbf(23) == Approx(6.3));
    REQUIRE(ts0.getRbf(24) == Approx(6.7));
    REQUIRE(ts0.getRbf(25) == Approx(6.7));
    REQUIRE(ts0.getRbf(26) == Approx(7.3));
}

TEST_CASE( "TaskSet::computeRbf() - TaskSet::checkSchedulability() - three (with offset)", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(0.2); f0.push_back(0.3); f0.push_back(0.4);
    Function ff0("ff0", 6, f0);
    Function ff1("ff1", 6, f0);
    
    std::vector<Function*> fs0;
    fs0.push_back(&ff0);
    
    Task t0; Task t1;
    t0.add(&ff0);
    t1.add(&ff1);
    t0.setOffset(2);
    TaskSet ts0;
    ts0.add(&t0); ts0.add(&t1);
    
    t0.changePriority(1);
    t1.changePriority(2);
    
    REQUIRE(ts0.checkSchedulability() == true);
    
    REQUIRE(t0.getFeta(0) == Approx(0));
    REQUIRE(t0.getFeta(2) == Approx(0.2));
    REQUIRE(t0.getFeta(8) == Approx(0.3));
    REQUIRE(t0.getFeta(14) == Approx(0.4));
    REQUIRE(t0.getFeta(20) == Approx(0.2));
    REQUIRE(t0.getFeta(26) == Approx(0.3));
    REQUIRE(t0.getFeta(32) == Approx(0.4));
    
    REQUIRE(t1.getFeta(0) == Approx(0.2));
    REQUIRE(t1.getFeta(6) == Approx(0.3));
    REQUIRE(t1.getFeta(12) == Approx(0.4));
    REQUIRE(t1.getFeta(18) == Approx(0.2));
    REQUIRE(t1.getFeta(24) == Approx(0.3));
    
    REQUIRE(ts0.getRbf(0) == Approx(0.2));
    REQUIRE(ts0.getRbf(2) == Approx(0.4));
    REQUIRE(ts0.getRbf(4) == Approx(0.4));
    REQUIRE(ts0.getRbf(6) == Approx(0.7));
    REQUIRE(ts0.getRbf(8) == Approx(1));
    REQUIRE(ts0.getRbf(10) == Approx(1));
    REQUIRE(ts0.getRbf(12) == Approx(1.4));
    REQUIRE(ts0.getRbf(14) == Approx(1.8));
    REQUIRE(ts0.getRbf(16) == Approx(1.8));
    REQUIRE(ts0.getRbf(18) == Approx(2));
    REQUIRE(ts0.getRbf(20) == Approx(2.2));
}

TEST_CASE( "TaskSet::computeRbf() - four (with offset)", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(0.2); f0.push_back(0.3); f0.push_back(0.4);
    Function ff0("ff0", 2, f0);
    
    std::vector<Function*> fs0;
    fs0.push_back(&ff0);
    
    Task t0; Task t1;
    t0.add(&ff0);
    t0.setOffset(6);
    t1.add(&ff0);
    TaskSet ts0;
    ts0.add(&t0);
    ts0.add(&t1);
    t0.changePriority(2);
    t1.changePriority(1);
    
    REQUIRE(ts0.checkSchedulability() == true);
    
    REQUIRE(ts0.getRbf(0) == Approx(0.2));
    REQUIRE(ts0.getRbf(2) == Approx(0.5));
    REQUIRE(ts0.getRbf(4) == Approx(0.9));
    REQUIRE(ts0.getRbf(6) == Approx(1.3));
    REQUIRE(ts0.getRbf(8) == Approx(1.9));
}

TEST_CASE( "TaskSet::checkSchedulability() - one", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(2);
    std::vector<float> f1; f1.push_back(1);
    
    Function ff0("ff0", 2, f0);
    Function ff1("ff1", 2, f1);
    
    Task t0; Task t1;
    t0.add(&ff0);
    t1.add(&ff1);
    t0.changePriority(1);
    t1.changePriority(2);
    t0.setOffset(2);
    TaskSet ts0;
    ts0.add(&t0); ts0.add(&t1);
    
    REQUIRE(ts0.checkSchedulability() == false);
}

TEST_CASE( "TaskSet::checkSchedulability() - TaskSet::getResponseTime(Function*) - one", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(1);
    std::vector<float> f1; f1.push_back(1);
    
    Function ff0("ff0", 2, f0);
    Function ff1("ff1", 2, f1);
    
    Task t0; Task t1;
    t0.add(&ff0);
    t1.add(&ff1);
    t0.changePriority(1);
    t1.changePriority(2);
    t1.setOffset(2);
    TaskSet ts0;
    ts0.add(&t0); ts0.add(&t1);
    
    REQUIRE(ts0.checkSchedulability() == true);
    
    REQUIRE(ts0.getResponseTime(&ff0) == 1);
    REQUIRE(ts0.getResponseTime(&ff1) == 2);
}

TEST_CASE( "TaskSet::computeRbf() - TaskSet::checkSchedulability() - four (with offset)", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(3);
    
    Function ff0("ff0", 8, f0);
    Task t0;
    t0.add(&ff0);
    t0.changePriority(1);
    t0.setOffset(6);
    TaskSet ts0;
    ts0.add(&t0);
    
    REQUIRE(ts0.checkSchedulability() == true);

    REQUIRE(ts0.getRbf(0) == 0);
    REQUIRE(ts0.getRbf(2) == 0);
    REQUIRE(ts0.getRbf(4) == 0);
    REQUIRE(ts0.getRbf(6) == 3);
    REQUIRE(ts0.getRbf(8) == 3);
    REQUIRE(ts0.getRbf(10) == 3);
    REQUIRE(ts0.getRbf(12) == 3);
    REQUIRE(ts0.getRbf(14) == 6);
    REQUIRE(ts0.getRbf(16) == 6);
    REQUIRE(ts0.getRbf(18) == 6);
    REQUIRE(ts0.getRbf(20) == 6);
    REQUIRE(ts0.getRbf(22) == 9);
    REQUIRE(ts0.getRbf(24) == 9);
    REQUIRE(ts0.getRbf(26) == 9);
    REQUIRE(ts0.getRbf(28) == 9);
    REQUIRE(ts0.getRbf(30) == 12);
    REQUIRE(ts0.getRbf(32) == 12);
    REQUIRE(ts0.getRbf(34) == 12);
    REQUIRE(ts0.getRbf(36) == 12);
}

TEST_CASE( "TaskSet::computeRbf() - TaskSet::checkSchedulability() - five (with offset)", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(3);
    
    Function ff0("ff0", 8, f0);
    Task t0;
    t0.add(&ff0);
    t0.changePriority(1);
    t0.setOffset(5);
    TaskSet ts0;
    ts0.add(&t0);
    
    REQUIRE(ts0.checkSchedulability() == true);
    
    REQUIRE(ts0.getRbf(0) == 0);
    REQUIRE(ts0.getRbf(1) == 0);
    REQUIRE(ts0.getRbf(4) == 0);
    REQUIRE(ts0.getRbf(5) == 3);
    REQUIRE(ts0.getRbf(12) == 3);
    REQUIRE(ts0.getRbf(13) == 6);
    REQUIRE(ts0.getRbf(20) == 6);
    REQUIRE(ts0.getRbf(21) == 9);
    REQUIRE(ts0.getRbf(28) == 9);
    REQUIRE(ts0.getRbf(29) == 12);

}

TEST_CASE( "TaskSet::computeRbf() - TaskSet::checkSchedulability() - six (with offset)", "[taskset]" )
{
    std::vector<float> f0; f0.push_back(3);
    std::vector<float> f1; f1.push_back(2);
    std::vector<float> f2; f2.push_back(1);
    
    Function ff0("ff0", 8, f0);
    Function ff1("ff1", 6, f1);
    Function ff2("fff2", 8, f2);
    
    Task t0; Task t1; Task t2;
    t0.add(&ff0);
    t1.add(&ff1);
    t2.add(&ff2);
    t0.changePriority(1);
    t1.changePriority(2);
    t2.changePriority(3);
    t2.setOffset(2);
    TaskSet ts0;
    ts0.add(&t0); ts0.add(&t1); ts0.add(&t2);
    
    REQUIRE(ts0.checkSchedulability() == true);
    
    REQUIRE(ts0.getResponseTime(&ff0) == 3);
    REQUIRE(ts0.getResponseTime(&ff1) == 5);
    REQUIRE(ts0.getResponseTime(&ff2) == 4);
    REQUIRE(ts0.getRbf(0) == 5);
    REQUIRE(ts0.getRbf(2) == 6);
    REQUIRE(ts0.getRbf(4) == 6);
    REQUIRE(ts0.getRbf(6) == 8);
    REQUIRE(ts0.getRbf(8) == 11);
    REQUIRE(ts0.getRbf(10) == 12);
    REQUIRE(ts0.getRbf(12) == 14);
    REQUIRE(ts0.getRbf(14) == 14);
    REQUIRE(ts0.getRbf(16) == 17);
    REQUIRE(ts0.getRbf(18) == 20);
    REQUIRE(ts0.getRbf(20) == 20);
    REQUIRE(ts0.getRbf(22) == 20);
    REQUIRE(ts0.getRbf(24) == 25);
    REQUIRE(ts0.getRbf(26) == 26);
}

TEST_CASE( "Function graph: test_1 - one", "[testbed]")
{
    std::vector<float> v0, v1;
    v0.push_back(1); v0.push_back(0); v0.push_back(0); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0); v0.push_back(0.373631); v0.push_back(0); v0.push_back(0.146518); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0); v0.push_back(0.186815); v0.push_back(0.293036); v0.push_back(0); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0);
    v1.push_back(1.33333); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.116967); v1.push_back(0.00393334); v1.push_back(0); v1.push_back(0); v1.push_back(0.945562); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.241468); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.726836); v1.push_back(0); v1.push_back(0); v1.push_back(0.00131111); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.950806); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.116967); v1.push_back(0.00393334); v1.push_back(0); v1.push_back(0); v1.push_back(1.32809); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.241468); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.709171); v1.push_back(0); v1.push_back(0); v1.push_back(0.00131111); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.974359); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.116967); v1.push_back(0.00393334); v1.push_back(0); v1.push_back(0); v1.push_back(0.945562); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.241468); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.996067); v1.push_back(0); v1.push_back(0); v1.push_back(0.00131111); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.950806); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.116967); v1.push_back(0.00393334); v1.push_back(0); v1.push_back(0); v1.push_back(0.969114); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.241468); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.709171); v1.push_back(0); v1.push_back(0); v1.push_back(0.00131111); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0);
    
    Function f0_0("t0_0", 1, v0);
    Function f0_1("t0_1", 1, v1);
    Task t0_0;
    t0_0.add(&f0_0);
    t0_0.add(&f0_1);
    t0_0.changePriority(1);
    TaskSet ts0;
    ts0.add(&t0_0);
    REQUIRE(ts0.checkSchedulability() == true);
}

TEST_CASE( "Function graph: test_1 - two", "[testbed]")
{
    std::vector<float> v0, v3;
    v0.push_back(1); v0.push_back(0); v0.push_back(0); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0); v0.push_back(0.373631); v0.push_back(0); v0.push_back(0.146518); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0); v0.push_back(0.186815); v0.push_back(0.293036); v0.push_back(0); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0); v0.push_back(0.560446); v0.push_back(0); v0.push_back(0);
    v3.push_back(1.33333); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.554403); v3.push_back(1.18546); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.554403); v3.push_back(0.406533); v3.push_back(1.18546); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.554403); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(1.18546); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.554403); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(1.18546); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.554403); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(1.18546); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.554403); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(1.18546); v3.push_back(0.406533); v3.push_back(0.554403); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(1.18546); v3.push_back(0.554403); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533); v3.push_back(0.406533);
    
    Function f0_0("t0_0", 1, v0);
    Function f0_3("t0_3", 4, v3);
    TaskSet ts0;
    
    ts0.addFunction(&f0_0);
    ts0.addFunction(&f0_3);
    ts0.getTask(&f0_3)->setOffset(1);
    REQUIRE(ts0.checkSchedulability() == true);
    
}

TEST_CASE( "Function graph: test_1 - three", "[testbed]")
{
    std::vector<float> v1, v2, v4;
    v1.push_back(1.33333); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.116967); v1.push_back(0.00393334); v1.push_back(0); v1.push_back(0); v1.push_back(0.945562); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.241468); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.726836); v1.push_back(0); v1.push_back(0); v1.push_back(0.00131111); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.950806); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.116967); v1.push_back(0.00393334); v1.push_back(0); v1.push_back(0); v1.push_back(1.32809); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.241468); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.709171); v1.push_back(0); v1.push_back(0); v1.push_back(0.00131111); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.974359); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.116967); v1.push_back(0.00393334); v1.push_back(0); v1.push_back(0); v1.push_back(0.945562); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.241468); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.996067); v1.push_back(0); v1.push_back(0); v1.push_back(0.00131111); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.950806); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.116967); v1.push_back(0.00393334); v1.push_back(0); v1.push_back(0); v1.push_back(0.969114); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.241468); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.709171); v1.push_back(0); v1.push_back(0); v1.push_back(0.00131111); v1.push_back(0.233935); v1.push_back(0); v1.push_back(0.238846); v1.push_back(0); v1.push_back(0.46787); v1.push_back(0); v1.push_back(0); v1.push_back(0);
    v2.push_back(2.66667); v2.push_back(0); v2.push_back(0.505467); v2.push_back(0.218867); v2.push_back(1.54213); v2.push_back(0); v2.push_back(2.13547); v2.push_back(0); v2.push_back(0.771064); v2.push_back(0.218867); v2.push_back(1.01093); v2.push_back(0);
    v4.push_back(2); v4.push_back(0.276221);
    
    Function f0_1("t0_1", 1, v1);
    Function f0_2("t0_2", 4, v2);
    Function f0_4("t0_4", 6, v4);
    TaskSet ts0;
    ts0.addFunction(&f0_1);
    ts0.addFunction(&f0_2);
    ts0.addFunction(&f0_4);
    
    REQUIRE(ts0.checkSchedulability() == true);
    
}

TEST_CASE( "Function graph: test_1 - four", "[testbed]")
{
    std::vector<float> v1, v2;
    v1.push_back(3.55556); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.636923); v1.push_back(0); v1.push_back(0.311913); v1.push_back(0.0104889); v1.push_back(0); v1.push_back(0); v1.push_back(2.5215); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.643915); v1.push_back(0); v1.push_back(1.24765); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(1.93823); v1.push_back(0); v1.push_back(0); v1.push_back(0.0034963); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.636923); v1.push_back(0); v1.push_back(1.24765); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(2.53548); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.636923); v1.push_back(0); v1.push_back(0.311913); v1.push_back(0.0104889); v1.push_back(0); v1.push_back(0); v1.push_back(3.54157); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.643915); v1.push_back(0); v1.push_back(1.24765); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(1.89112); v1.push_back(0); v1.push_back(0); v1.push_back(0.0034963); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.636923); v1.push_back(0); v1.push_back(1.24765); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(2.59829); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.636923); v1.push_back(0); v1.push_back(0.311913); v1.push_back(0.0104889); v1.push_back(0); v1.push_back(0); v1.push_back(2.5215); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.643915); v1.push_back(0); v1.push_back(1.24765); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(2.65618); v1.push_back(0); v1.push_back(0); v1.push_back(0.0034963); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.636923); v1.push_back(0); v1.push_back(1.24765); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(2.53548); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.636923); v1.push_back(0); v1.push_back(0.311913); v1.push_back(0.0104889); v1.push_back(0); v1.push_back(0); v1.push_back(2.5843); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.643915); v1.push_back(0); v1.push_back(1.24765); v1.push_back(0); v1.push_back(0); v1.push_back(0); v1.push_back(1.89112); v1.push_back(0); v1.push_back(0); v1.push_back(0.0034963); v1.push_back(0.623826); v1.push_back(0); v1.push_back(0.636923); v1.push_back(0); v1.push_back(1.24765); v1.push_back(0); v1.push_back(0); v1.push_back(0);
    v2.push_back(7.11111); v2.push_back(0); v2.push_back(1.34791); v2.push_back(0.583645); v2.push_back(4.11234); v2.push_back(0); v2.push_back(5.69459); v2.push_back(0); v2.push_back(2.05617); v2.push_back(0.583645); v2.push_back(2.69582); v2.push_back(0);
    
    Function f0_1("t0_0", 1, v1);
    Function f0_2("t0_3", 4, v2);
    TaskSet ts0;
    ts0.addFunction(&f0_1);
    ts0.addFunction(&f0_2);
    ts0.getTask(&f0_2)->setOffset(4);
    
    REQUIRE(ts0.checkSchedulability() == false);
    
}

TEST_CASE( "Function graph: test_1 - five", "[testbed]")
{
    std::vector<float> v7, v2;
    v7.push_back(14.2222); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0.290018); v7.push_back(5.60669); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0.580037); v7.push_back(0); v7.push_back(8.92656); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0.870055); v7.push_back(0); v7.push_back(0); v7.push_back(4.00478); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(1.16007); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(5.95104); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(1.45009); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(2.40287); v7.push_back(0); v7.push_back(0); v7.push_back(1.74011); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(2.97552); v7.push_back(0); v7.push_back(2.03013); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0.800956); v7.push_back(2.32015); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0); v7.push_back(0);
    v2.push_back(7.11111); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(0.752769); v2.push_back(2.79288); v2.push_back(0); v2.push_back(0); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(1.50554); v2.push_back(0); v2.push_back(1.86192); v2.push_back(0); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(2.54046); v2.push_back(0); v2.push_back(0); v2.push_back(0.930959); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(6.73491); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(0.752769); v2.push_back(2.79288); v2.push_back(0); v2.push_back(0); v2.push_back(3.38728); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(1.50554); v2.push_back(0); v2.push_back(1.86192); v2.push_back(0); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(2.25831); v2.push_back(0); v2.push_back(0); v2.push_back(0.930959); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0); v2.push_back(3.01108); v2.push_back(0); v2.push_back(0); v2.push_back(0);
    Function f0_7("t0_7", 2, v7);
    Function f0_2("t0_2", 2, v2);
    TaskSet ts0;
    ts0.addFunction(&f0_7);
    ts0.addFunction(&f0_2);
    
    REQUIRE(ts0.checkSchedulability() == false);
    
}

TEST_CASE( "Function graph: test_1 - six", "[testbed]")
{
    std::vector<float> v0;
    v0.push_back(1.77778); v0.push_back(0.970656); v0.push_back(0.970656); v0.push_back(0.970656); v0.push_back(0.970656); v0.push_back(0.970656); v0.push_back(0.970656); v0.push_back(0.970656); v0.push_back(0.970656); v0.push_back(0.970656);
    
    Function f0_0("t0_0", 1, v0);
    TaskSet ts0;
    ts0.addFunction(&f0_0);
    ts0.getTask(&f0_0)->setOffset(4);
    
    REQUIRE(ts0.checkSchedulability() == false);
    REQUIRE(ts0.getFeta(0) == 0);
    REQUIRE(ts0.getFeta(2) == 0);
    REQUIRE(ts0.getFeta(4) == Approx(1.77778));
    REQUIRE(ts0.getRbf(0) == 0);
    REQUIRE(ts0.getRbf(2) == 0);
    REQUIRE(ts0.getRbf(4) == Approx(1.77778));
    REQUIRE(ts0.getRbf(5) == Approx(2.748436));
    REQUIRE(ts0.getRbf(6) == Approx(3.719092));
}


