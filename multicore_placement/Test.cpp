///*
// F-Sched: Scheduling, Optimization and Placement of AUTOSAR Runables
// Copyright (C) <2014>  <Fabio Cremona> fabiocremona@gmail.com
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// */
//
////  Test.cpp
//
//#include <iostream>
//#include <fstream>
//#include <algorithm>
//
//
//#include "Parser.h"
//#include "TaskSet.h"
//#include "RtwOpt.h"
//#include "Processor.h"
//
//#define MULTIPLIER 99999999999999
//
//
//#define CATCH_CONFIG_MAIN
//#include "catch.h"
//
//TEST_CASE( "Task", "[feta creation from function's FETA]" ) {
//    
//    std::vector<float> v0;
//    std::vector<float> v1;
//    v0.push_back(2); v0.push_back(0); v0.push_back(1); v0.push_back(0);
//    v1.push_back(5); v1.push_back(0); v1.push_back(3); v1.push_back(0);
//    
//    Function f0("f0", 2, v0);
//    Function f1("f1", 2, v1);
//    Task t0;
//    t0.add(&f0); t0.add(&f1);
//    
//    REQUIRE(t0.get()[0] == 7);
//    REQUIRE(t0.get()[1] == 0);
//    REQUIRE(t0.get()[2] == 4);
//    REQUIRE(t0.get()[3] == 0);
//    REQUIRE(t0.getPeriod() == 2);
//    REQUIRE(t0.getSize() == 4);
//    REQUIRE(t0.getFunSize() == 2);
//    
//    t0.remove(&f1);
//    REQUIRE(t0.get()[0] == 2);
//    REQUIRE(t0.get()[1] == 0);
//    REQUIRE(t0.get()[2] == 1);
//    REQUIRE(t0.get()[3] == 0);
//    REQUIRE(t0.getPeriod() == 2);
//    REQUIRE(t0.getSize() == 4);
//    REQUIRE(t0.getFunSize() == 1);
//    
//    t0.remove(&f0);
//    REQUIRE(t0.getSize() == 0);
//    REQUIRE(t0.getFunSize() == 0);
//    
//    Function f2("f2", 3, v1);
//    t0.add(&f0); t0.add(&f2);
//    
//    REQUIRE(t0.get()[0] == 7);
//    REQUIRE(t0.get()[1] == 0);
//    REQUIRE(t0.get()[2] == 0);
//    REQUIRE(t0.get()[3] == 0);
//    REQUIRE(t0.get()[4] == 1);
//    REQUIRE(t0.get()[5] == 0);
//    REQUIRE(t0.get()[6] == 3);
//    REQUIRE(t0.get()[7] == 0);
//    REQUIRE(t0.get()[8] == 2);
//    REQUIRE(t0.getPeriod() == 1);
//    REQUIRE(t0.getSize() == 24);
//    REQUIRE(t0.getFunSize() == 2);
//}
//
//TEST_CASE( "TaskSet_0", "[Schedulability Check and TaskSet FETA]" ) {
//    
//    std::vector<float> v0;
//    std::vector<float> v1;
//    v0.push_back(1); v0.push_back(0); v0.push_back(1); v0.push_back(0);
//    v1.push_back(1); v1.push_back(0); v1.push_back(1); v1.push_back(0);
//    
//    Function f0("f0", 2, v0);
//    Function f1("f1", 1, v1);
//    Task t0; Task t1;
//    t0.add(&f0); t1.add(&f1);
//    TaskSet ts0;
//    
//    t0.changePriority(2);
//    t1.changePriority(1);
//    
//    ts0.add(&t0);
//    ts0.add(&t1);
//    
//    ts0.computeFeta();
//    
//    REQUIRE(ts0.getTs().size() == 2);
//    REQUIRE(ts0.getPeriod() == 1);
//    REQUIRE(ts0.getSize() == 8);
//    
//    REQUIRE(ts0.get()[0] == 2);
//    REQUIRE(ts0.get()[1] == 0);
//    REQUIRE(ts0.get()[2] == 1);
//    REQUIRE(ts0.get()[3] == 0);
//    REQUIRE(ts0.get()[4] == 2);
//    REQUIRE(ts0.get()[5] == 0);
//    REQUIRE(ts0.get()[6] == 1);
//    REQUIRE(ts0.get()[7] == 0);
//    
//    ts0.remove(&t1);
//    ts0.computeFeta();
//    REQUIRE(ts0.getTs().size() == 1);
//    REQUIRE(ts0.getPeriod() == 2);
//    
//    REQUIRE(ts0.getSize() == 4);
//    REQUIRE(ts0.get()[0] == 1);
//    REQUIRE(ts0.get()[1] == 0);
//    REQUIRE(ts0.get()[2] == 1);
//    REQUIRE(ts0.get()[3] == 0);
//    REQUIRE(ts0.checkSchedulability() == true);
//    
//    ts0.remove(&t0);
//    ts0.add(&t1);
//    ts0.computeFeta();
//    REQUIRE(ts0.getTs().size() == 1);
//    REQUIRE(ts0.getPeriod() == 1);
//    REQUIRE(ts0.getSize() == 4);
//    REQUIRE(ts0.get()[0] == 1);
//    REQUIRE(ts0.get()[1] == 0);
//    REQUIRE(ts0.get()[2] == 1);
//    REQUIRE(ts0.get()[3] == 0);
//    REQUIRE(ts0.checkSchedulability() == true);
//    
//    ts0.add(&t0);
//    REQUIRE(ts0.checkSchedulability() == true);
//    
//}
//
//
//TEST_CASE( "TaskSet_1", "[Schedulability Check and TaskSet FETA]" ) {
//    std::vector<float> v0;
//    std::vector<float> v1;
//    v0.push_back(1); v0.push_back(1); v0.push_back(1); v0.push_back(0);
//    v1.push_back(1); v1.push_back(0.5); v1.push_back(1); v1.push_back(0.5);
//    
//    Function f0("f0", 2, v0);
//    Function f1("f1", 1, v1);
//    Task t0; Task t1;
//    t0.add(&f0); t1.add(&f1);
//    TaskSet ts0;
//    
//    t0.changePriority(2);
//    t1.changePriority(1);
//    
//    ts0.add(&t0);
//    ts0.add(&t1);
//    
//    REQUIRE(ts0.checkSchedulability() == false);
//    
//}
//
//TEST_CASE( "Two affine runnables on the same processor (with bandwith)", "t0" ) {
//    
//    std::vector<float> f0; f0.push_back(0.3);
//    std::vector<float> f1; f1.push_back(0.2);
//    std::vector<float> f2; f2.push_back(1);
//    Component c0("");
//    Function ff0("ff0", 1, f0); Function ff1("ff1", 1, f1);
//    c0.addFunction(&ff0); c0.addFunction(&ff1);
//    c0.connect(&ff0, &ff1);
//    std::vector<Function*> fs0;
//    fs0.push_back(&ff0); fs0.push_back(&ff1);
//    
//    Processor p0(fs0, 2);
////    p0.start();
////    
////    REQUIRE(p0.getCoreIdx(&ff0) == 0);
////    REQUIRE(p0.getCoreIdx(&ff1) == 0);
//}
//
//TEST_CASE( "Two affine runnable in one processor, one single dense runnable on another", "t0" ) {
//    
//    std::vector<float> f0; f0.push_back(0.3);
//    std::vector<float> f1; f1.push_back(0.2);
//    std::vector<float> f2; f2.push_back(1);
//    Component c0("");
//    Function ff0("ff0", 1, f0); Function ff1("ff1", 1, f1); Function ff2("ff2", 1, f2);
//    c0.addFunction(&ff0); c0.addFunction(&ff1); c0.addFunction(&ff2);
//    c0.connect(&ff0, &ff1);
//    std::vector<Function*> fs0;
//    fs0.push_back(&ff0); fs0.push_back(&ff1); fs0.push_back(&ff2);
//    
//    Processor p0(fs0, 2);
////    p0.start();
////    
////    REQUIRE(p0.getCoreIdx(&ff0) == 1);
////    REQUIRE(p0.getCoreIdx(&ff1) == 1);
////    REQUIRE(p0.getCoreIdx(&ff2) == 0);
//}
//
//TEST_CASE( "Two affine runnable in one processor, one single runnable on another", "t0" ) {
//    
//    std::vector<float> f0; f0.push_back(0.3);
//    std::vector<float> f1; f1.push_back(0.2);
//    std::vector<float> f2; f2.push_back(1);
//    Component c0("");
//    Function ff0("ff0", 1, f0); Function ff1("ff1", 1, f1); Function ff2("ff2", 5, f2);
//    c0.addFunction(&ff0); c0.addFunction(&ff1); c0.addFunction(&ff2);
//    c0.connect(&ff0, &ff1);
//    std::vector<Function*> fs0;
//    fs0.push_back(&ff0); fs0.push_back(&ff1); fs0.push_back(&ff2);
//    
//    Processor p0(fs0, 2);
////    p0.start();
////    
////    REQUIRE(p0.getCoreIdx(&ff0) == 0);
////    REQUIRE(p0.getCoreIdx(&ff1) == 0);
////    REQUIRE(p0.getCoreIdx(&ff2) == 1);
//}
//
//TEST_CASE( "the second affine does not fit", "t0" ) {
//    
//    std::vector<float> f0; f0.push_back(0.9);
//    std::vector<float> f1; f1.push_back(0.2);
//    std::vector<float> f2; f2.push_back(1);
//    Component c0("");
//    Function ff0("ff0", 1, f0); Function ff1("ff1", 1, f1); Function ff2("ff2", 5, f2);
//    c0.addFunction(&ff0); c0.addFunction(&ff1); c0.addFunction(&ff2);
//    c0.connect(&ff0, &ff1);
//    std::vector<Function*> fs0;
//    fs0.push_back(&ff0); fs0.push_back(&ff1); fs0.push_back(&ff2);
//    
//    Processor p0(fs0, 2);
////    p0.start();
////
////    REQUIRE(p0.getCoreIdx(&ff0) == 0);
////    REQUIRE(p0.getCoreIdx(&ff1) == 1);
////    REQUIRE(p0.getCoreIdx(&ff2) == 1);
//}
//
//TEST_CASE("synchronous set test", "test one") {
//    std::vector<float> f0; f0.push_back(0.2);
//    std::vector<float> f1; f1.push_back(0.2);
//    std::vector<float> f2; f2.push_back(0.2);
//    std::vector<float> f3; f3.push_back(0.2);
//    std::vector<float> f4; f4.push_back(0.2);
//    std::vector<float> f5; f5.push_back(0.2);
//    Function ff0("ff0", 2, f0); Function ff1("ff1", 2, f1); Function ff2("ff2", 2, f2);
//    Function ff3("ff3", 2, f3); Function ff4("ff4", 2, f4); Function ff5("ff5", 2, f5); Function ff6("ff6", 3, f5); Function ff7("ff7", 2, f5);
//    
//    std::vector<Function*> fs0; std::vector<Function*> fs1;
//    fs0.push_back(&ff0); fs0.push_back(&ff1); fs0.push_back(&ff2); fs0.push_back(&ff7);
//    fs1.push_back(&ff3); fs1.push_back(&ff4); fs1.push_back(&ff5); fs1.push_back(&ff6);
//    
//    Component c0("");
//    c0.addFunction(&ff0); c0.addFunction(&ff1); c0.addFunction(&ff2); c0.addFunction(&ff3); c0.addFunction(&ff4); c0.addFunction(&ff5); c0.addFunction(&ff6);  c0.addFunction(&ff7);
//    c0.connect(&ff0, &ff5); c0.connect(&ff2, &ff5); c0.connect(&ff4, &ff5); c0.connect(&ff6, &ff5);  c0.connect(&ff5, &ff7);
//    
//    Task t0; Task t1;
//    t0.add(&fs0);
//    t1.add(&fs1);
//    t0.changePriority(2);
//    t1.changePriority(1);
//    TaskSet ts0;
//    ts0.add(&t0); ts0.add(&t1);
//    RtwOpt opt0(ts0);
//    
//    auto ss = opt0.getSS(&ff5);
//    
//    REQUIRE(ss.size() == 5);
//    REQUIRE(std::find(ss.begin(), ss.end(), &ff5) != ss.end());
//    REQUIRE(std::find(ss.begin(), ss.end(), &ff0) != ss.end());
//    REQUIRE(std::find(ss.begin(), ss.end(), &ff2) != ss.end());
//    REQUIRE(std::find(ss.begin(), ss.end(), &ff4) != ss.end());
//    REQUIRE(std::find(ss.begin(), ss.end(), &ff7) != ss.end());
//    
//}
//
//TEST_CASE( "rbf test", "test 0" ) {
//    std::vector<float> f0; f0.push_back(0.2);
//    std::vector<float> f1; f1.push_back(0.2);
//    std::vector<float> f2; f2.push_back(0.2);
//    std::vector<float> f3; f3.push_back(0.2);
//    std::vector<float> f4; f4.push_back(0.2);
//    std::vector<float> f5; f5.push_back(0.2);
//    Function ff0("ff0", 2, f0); Function ff1("ff1", 2, f1); Function ff2("ff2", 2, f2);
//    Function ff3("ff3", 1, f3); Function ff4("ff4", 1, f4); Function ff5("ff5", 1, f5);
//    std::vector<Function*> fs0; std::vector<Function*> fs1;
//    fs0.push_back(&ff0); fs0.push_back(&ff1); fs0.push_back(&ff2);
//    fs1.push_back(&ff3); fs1.push_back(&ff4); fs1.push_back(&ff5);
//    
//    Task t0; Task t1;
//    t0.add(&fs0);
//    t1.add(&fs1);
//    t0.changePriority(2);
//    t1.changePriority(1);
//    TaskSet ts0;
//    ts0.add(&t0); ts0.add(&t1);
//    ts0.checkSchedulability();
//    REQUIRE(ts0.getRbf(0) == Approx(1.2));
//    REQUIRE(ts0.getRbf(4) == Approx(4.8));
//}
//
//TEST_CASE("test rt size equivalence", "") {
//    std::vector<float> f0; f0.push_back(0.2);
//    std::vector<float> f1; f1.push_back(0.2);
//    std::vector<float> f2; f2.push_back(0.2);
//    std::vector<float> f3; f3.push_back(0.2);
//    std::vector<float> f4; f4.push_back(0.2);
//    std::vector<float> f5; f5.push_back(0.2);
//    Function ff0("ff0", 2, f0); Function ff1("ff1", 2, f1); Function ff2("ff2", 2, f2);
//    Function ff3("ff3", 2, f3); Function ff4("ff4", 2, f4); Function ff5("ff5", 2, f5); Function ff6("ff6", 3, f5); Function ff7("ff7", 2, f5);
//    
//    std::vector<Function*> fs0; std::vector<Function*> fs1;
//    fs0.push_back(&ff0); fs0.push_back(&ff1); fs0.push_back(&ff2); fs0.push_back(&ff7);
//    fs1.push_back(&ff3); fs1.push_back(&ff4); fs1.push_back(&ff5); fs1.push_back(&ff6);
//    
//    Component c0("");
//    c0.addFunction(&ff0); c0.addFunction(&ff1); c0.addFunction(&ff2); c0.addFunction(&ff3); c0.addFunction(&ff4); c0.addFunction(&ff5); c0.addFunction(&ff6);  c0.addFunction(&ff7);
//    c0.connect(&ff0, &ff5); c0.connect(&ff2, &ff5); c0.connect(&ff4, &ff5); c0.connect(&ff6, &ff5);  c0.connect(&ff5, &ff7);
//    
//    Task t0; Task t1;
//    t0.add(&fs0);
//    t1.add(&fs1);
//    t0.changePriority(2);
//    t1.changePriority(1);
//    TaskSet ts0;
//    ts0.add(&t0); ts0.add(&t1);
//    
//    RtwOpt opt0(ts0);
//    ts0.computeRT();
//
//    REQUIRE(opt0.getRT2() == ts0.getRT2());
//}
//
///////////
//
//TEST_CASE("feta with offset", "") {
////    std::cout << "!!!FETA WITH OFFSET!!!" << std::endl;
//    std::vector<float> f0; f0.push_back(0.2); f0.push_back(0.3); f0.push_back(0.4);
//    Function ff0("ff0", 2, f0);
//    
//    std::vector<Function*> fs0;
//    fs0.push_back(&ff0);
//
//    Task t0; Task t1;
//    t0.add(&ff0);
//    t1.add(&ff0);
//    t0.setOffset(6);
//    TaskSet ts0;
//    ts0.add(&t0); ts0.add(&t1);
//    ts0.checkSchedulability();
//
//    REQUIRE(t0.getFeta(0) == Approx(0));
//    REQUIRE(t0.getFeta(2) == Approx(0));
//    REQUIRE(t0.getFeta(4) == Approx(0));
//    REQUIRE(t0.getFeta(6) == Approx(0.2));
//    REQUIRE(t0.getFeta(8) == Approx(0.3));
//    REQUIRE(t0.getFeta(10) == Approx(0.4));
//    REQUIRE(t0.getFeta(12) == Approx(0.2));
//    REQUIRE(t0.getFeta(14) == Approx(0.3));
//    REQUIRE(t0.getFeta(16) == Approx(0.4));
//    REQUIRE(t0.getFeta(18) == Approx(0.2));
//    REQUIRE(t0.getFeta(20) == Approx(0.3));
//    REQUIRE(t0.getFeta(22) == Approx(0.4));
//    REQUIRE(ts0.getRbf(0) == Approx(0.2));
//    REQUIRE(ts0.getRbf(2) == Approx(0.5));
//}
//
//TEST_CASE("feta with offset0", "") {
//    std::vector<float> f0; f0.push_back(0.2); f0.push_back(0.3); f0.push_back(0.4);
//    Function ff0("ff0", 6, f0);
//    Function ff1("ff1", 6, f0);
//    
//    std::vector<Function*> fs0;
//    fs0.push_back(&ff0);
//    
//    Task t0; Task t1;
//    t0.add(&ff0);
//    t1.add(&ff1);
//    t0.setOffset(2);
//    TaskSet ts0;
//    ts0.add(&t0); ts0.add(&t1);
//
//    std::cout << std::endl;
//    t0.changePriority(1);
//    t1.changePriority(2);
//    ts0.checkSchedulability();
//
//    REQUIRE(t0.getFeta(0) == Approx(0));
//    REQUIRE(t0.getFeta(2) == Approx(0.2));
//    REQUIRE(t0.getFeta(8) == Approx(0.3));
//    REQUIRE(t0.getFeta(14) == Approx(0.4));
//    REQUIRE(t0.getFeta(20) == Approx(0.2));
//    REQUIRE(t0.getFeta(26) == Approx(0.3));
//    REQUIRE(t0.getFeta(32) == Approx(0.4));
//    
//    REQUIRE(t1.getFeta(0) == Approx(0.2));
//    REQUIRE(t1.getFeta(6) == Approx(0.3));
//    REQUIRE(t1.getFeta(12) == Approx(0.4));
//    REQUIRE(t1.getFeta(18) == Approx(0.2));
//    REQUIRE(t1.getFeta(24) == Approx(0.3));
//
//    REQUIRE(ts0.getRbf(0) == Approx(0.2));
//    REQUIRE(ts0.getRbf(2) == Approx(0.4));
//    REQUIRE(ts0.getRbf(4) == Approx(0.4));
//    REQUIRE(ts0.getRbf(6) == Approx(0.7));
//    REQUIRE(ts0.getRbf(8) == Approx(1));
//    REQUIRE(ts0.getRbf(10) == Approx(1));
//    REQUIRE(ts0.getRbf(12) == Approx(1.4));
//    REQUIRE(ts0.getRbf(14) == Approx(1.8));
//    REQUIRE(ts0.getRbf(16) == Approx(1.8));
//    REQUIRE(ts0.getRbf(18) == Approx(2));
//    REQUIRE(ts0.getRbf(20) == Approx(2.2));
//
//}
//
//TEST_CASE("rbf test for feta with offset", "") {
//    std::vector<float> f0; f0.push_back(0.2); f0.push_back(0.3); f0.push_back(0.4);
//    Function ff0("ff0", 2, f0);
//    
//    std::vector<Function*> fs0;
//    fs0.push_back(&ff0);
//    
//    Task t0; Task t1;
//    t0.add(&ff0);
//    t0.setOffset(6);
//    t1.add(&ff0);
//    TaskSet ts0;
//    ts0.add(&t0);
//    ts0.add(&t1);
//    t0.changePriority(2);
//    t1.changePriority(1);
//    ts0.checkSchedulability();
//
//    REQUIRE(ts0.getRbf(0) == Approx(0.2));
//    REQUIRE(ts0.getRbf(2) == Approx(0.5));
//    REQUIRE(ts0.getRbf(4) == Approx(0.9));
//    REQUIRE(ts0.getRbf(6) == Approx(1.3));
//    REQUIRE(ts0.getRbf(8) == Approx(1.9));
//}
//
//TEST_CASE("sched test with offset 0", "") {
//    std::vector<float> f0; f0.push_back(2);
//    std::vector<float> f1; f1.push_back(1);
//    
//    Function ff0("ff0", 2, f0);
//    Function ff1("ff1", 2, f1);
//    
//    Task t0; Task t1;
//    t0.add(&ff0);
//    t1.add(&ff1);
//    t0.changePriority(1);
//    t1.changePriority(2);
//    t0.setOffset(2);
//    TaskSet ts0;
//    ts0.add(&t0); ts0.add(&t1);
//    
//    REQUIRE(ts0.checkSchedulability() == false);
//}
//
//TEST_CASE("sched test with offset 1", "") {
//    std::vector<float> f0; f0.push_back(1);
//    std::vector<float> f1; f1.push_back(1);
//    
//    Function ff0("ff0", 2, f0);
//    Function ff1("ff1", 2, f1);
//    
//    Task t0; Task t1;
//    t0.add(&ff0);
//    t1.add(&ff1);
//    t0.changePriority(1);
//    t1.changePriority(2);
//    t1.setOffset(2);
//    TaskSet ts0;
//    ts0.add(&t0); ts0.add(&t1);
//
//    REQUIRE(ts0.checkSchedulability() == true);
//    REQUIRE(ts0.getResponseTime(&ff0) == 1);
//    REQUIRE(ts0.getResponseTime(&ff1) == 2);
//}
//
//TEST_CASE("sched test with offset 2", "") {
//    std::vector<float> f0; f0.push_back(3);
//    std::vector<float> f1; f1.push_back(2);
//    std::vector<float> f2; f2.push_back(1);
//    
//    Function ff0("ff0", 8, f0);
//    Function ff1("ff1", 6, f1);
//    Function ff2("ff2", 8, f2);
//    
//    Task t0; Task t1; Task t2;
//    t0.add(&ff0);
//    t1.add(&ff1);
//    t2.add(&ff2);
//    t0.changePriority(1);
//    t1.changePriority(2);
//    t2.changePriority(3);
//    t2.setOffset(2);
//    TaskSet ts0;
//    ts0.add(&t0); ts0.add(&t1); ts0.add(&t2);
//    
//    REQUIRE(ts0.checkSchedulability() == true);
//    REQUIRE(ts0.getResponseTime(&ff0) == 3);
//    REQUIRE(ts0.getResponseTime(&ff1) == 5);
//    REQUIRE(ts0.getResponseTime(&ff2) == 4);
//    REQUIRE(ts0.getRbf(0) == 5);
//    REQUIRE(ts0.getRbf(2) == 6);
//    REQUIRE(ts0.getRbf(4) == 6);
//    REQUIRE(ts0.getRbf(6) == 8);
//    REQUIRE(ts0.getRbf(8) == 11);
//    REQUIRE(ts0.getRbf(10) == 12);
//    REQUIRE(ts0.getRbf(12) == 14);
//    REQUIRE(ts0.getRbf(14) == 14);
//    REQUIRE(ts0.getRbf(16) == 17);
//    REQUIRE(ts0.getRbf(18) == 20);
//    REQUIRE(ts0.getRbf(20) == 20);
//    REQUIRE(ts0.getRbf(22) == 20);
//    REQUIRE(ts0.getRbf(24) == 25);
//    REQUIRE(ts0.getRbf(26) == 26);
//
//}
//
//TEST_CASE("sched test: only one task with offset", "") {
//    std::vector<float> f0; f0.push_back(3);
//    
//    Function ff0("ff0", 8, f0);
//    Task t0;
//    t0.add(&ff0);
//
//    t0.changePriority(1);
//
//    t0.setOffset(6);
//    TaskSet ts0;
//    ts0.add(&t0);
//    
//    REQUIRE(ts0.checkSchedulability() == true);
//    ts0.checkSchedulability();
//    REQUIRE(ts0.getRbf(0) == 0);
//    REQUIRE(ts0.getRbf(2) == 0);
//    REQUIRE(ts0.getRbf(4) == 0);
//    REQUIRE(ts0.getRbf(6) == 3);
//    REQUIRE(ts0.getRbf(8) == 3);
//    REQUIRE(ts0.getRbf(10) == 3);
//    REQUIRE(ts0.getRbf(12) == 3);
//    REQUIRE(ts0.getRbf(14) == 6);
//    REQUIRE(ts0.getRbf(16) == 6);
//    REQUIRE(ts0.getRbf(18) == 6);
//    REQUIRE(ts0.getRbf(20) == 6);
//    REQUIRE(ts0.getRbf(22) == 9);
//    REQUIRE(ts0.getRbf(24) == 9);
//    REQUIRE(ts0.getRbf(26) == 9);
//    REQUIRE(ts0.getRbf(28) == 9);
//    REQUIRE(ts0.getRbf(30) == 12);
//    REQUIRE(ts0.getRbf(32) == 12);
//    REQUIRE(ts0.getRbf(34) == 12);
//    REQUIRE(ts0.getRbf(36) == 12);
//}
//
//TEST_CASE("sched test: only one task with offset odd", "") {
//    std::vector<float> f0; f0.push_back(3);
//    
//    Function ff0("ff0", 8, f0);
//    Task t0;
//    t0.add(&ff0);
//    
//    t0.changePriority(1);
//    
//    t0.setOffset(5);
//    TaskSet ts0;
//    ts0.add(&t0);
//    
//    REQUIRE(ts0.checkSchedulability() == true);
//    ts0.checkSchedulability();
//    REQUIRE(ts0.getRbf(0) == 0);
//    REQUIRE(ts0.getRbf(2) == 0);
//    REQUIRE(ts0.getRbf(4) == 0);
//    REQUIRE(ts0.getRbf(5) == 3);
//    REQUIRE(ts0.getRbf(6) == 3);
//    REQUIRE(ts0.getRbf(12) == 3);
//    REQUIRE(ts0.getRbf(13) == 6);
//    REQUIRE(ts0.getRbf(14) == 6);
//    REQUIRE(ts0.getRbf(16) == 6);
//    REQUIRE(ts0.getRbf(18) == 6);
//    REQUIRE(ts0.getRbf(20) == 9);
//}
//
//
//
