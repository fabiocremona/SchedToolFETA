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
//  File: Parser.cpp

#include "Parser.h"
#include <stdlib.h>
#include <algorithm>
#define scale_factor 1

Parser::Parser(std::string file)
{
    source_file = file;
    Up = (double)rand() / (double)RAND_MAX;
    Up = 0.8 + (-0.15 + Up * 0.3);
    tasks = new std::vector<Task* >;
}


Parser::Parser(std::string file, float ub)
{
    source_file = file;
    Up = ub;
    tasks = new std::vector<Task* >;
}

Parser::Parser(std::string file, std::string up)
{
    source_file = file;
	//check whether up is a number or not. if not, it's the file for fecta
	bool is_number = false;
	std::string::const_iterator it = up.begin();
	while(it != up.end())
	{
		if(!std::isdigit(*it) && (*it) != '.')
		{
			break;
		}
		else
		{
			it++;
		}
	}
	if(it==up.end() && !up.empty())
	{
		is_number = true;
		Up = (float)atof(up.c_str());
		tasks = new std::vector<Task* >;
		create();
		//std::cout<<"use randomly generated input"<<std::endl;
	}
	else
	{
		feta_file = up;
		tasks = new std::vector<Task* >;
		createFromFile();
		std::cout<<"use user input: "<<feta_file<<std::endl;
	}
    //Up = (float)atof(up.c_str());
	//exit(1);
}

Parser::~Parser()
{
    for (auto component : components)
        delete component;
    for (auto function : functions)
        delete function.second;
}

void Parser::setUb()
{
    auto n_fun = functions.size();
    
    std::vector<float> u;
    for (auto i = functions.begin(); i != functions.end(); i++)
    {
        auto p = -0.2 + 0.4*(float)rand() / (float)RAND_MAX;
        auto t = (Up / n_fun);
        u.push_back(t + t * p);
    }
    float T = 0;
    for (auto e : u)
        T += e;
    
    auto F = Up/T;
    
    auto i = 0;
    for (auto f : functions)
        f.second->setUtilizationBound(u[i++] * F);
}

void Parser::create()
{
    std::ifstream source;
    std::string line;
    
    try {
        source.open(source_file.c_str());
        
    } catch (...) {
        throw "Unable to open source file";
    }
    
    while (source.good()) {
        std::string  type, name;
        getline(source, line);
        std::istringstream linestream(line);
        linestream >> type;
        
        if (type == "@TASK_GRAPH") {
            linestream >> name;
            Component* comp_tmp = new Component(name);
            components.push_back(comp_tmp);
        }
        if (type == "TASK") {
            linestream >> name;
            Function* fun_tmp = new Function(name);
            functions[name] = fun_tmp;
            components.back()->addFunction(fun_tmp);
        }
        if (type == "ARC") {
            std::string from, to;
            linestream >> from >> from >> from;
            linestream >> to >> to;
            components.back()->connect(functions[from], functions[to]);
        }
    }
    source.close();
    
    // Set components utilization
    
    setUb();
    checkFunctions();
}

//Peng
void Parser::createFromFile()
{
    
    std::ifstream source;
    std::string line;
    
    try {
        source.open(source_file.c_str());
        
    } catch (...) {
        throw "unable to open mapping_file file";
    }
    
    Component* comp_tmp = new Component("Default");
	components.push_back(comp_tmp);
    //add_component(_comp_tmp);
    while (source.good()) {
        std::string  type, name;
        long period;
        
        getline(source, line);
        std::istringstream linestream(line);
        linestream >> type;
        
        if (type == "FUNCTION") {
            linestream >> name;
            linestream >> period;
            
			std::vector<float> places_tmp;
			Function* fun_tmp = new Function(name,period,places_tmp);
			functions[name] = fun_tmp;
            components.back()->addFunction(fun_tmp);
        }
        if (type == "ARC") {
            std::string from, to;
			long bus_size;
            linestream >> from;
            linestream >> to;
			linestream >> bus_size;
			//components.back()->connect(functions[from], functions[to]);
			components.back()->connect(functions[from], functions[to], bus_size);
        }
    }
    source.close();
	
	std::ifstream feta_source;
    std::string _line1;
    try {
        feta_source.open(feta_file.c_str());
        
    } catch (...) {
        throw "unable to open _feta_file file";
    }
    while (feta_source.good()) {
        std::string _name, _tmp;
        float _val;
        getline(feta_source, _line1);
        std::istringstream _linestream(_line1);
        
        _linestream >> _tmp;
        if (_tmp == "-") {
            _linestream >> _name;
            
            //std::vector<Location *> *_feta = new std::vector<Location *>;
            std::vector<float> feta;
            
            while (_linestream.good()) {
                
                _linestream >> _val;
				feta.push_back(_val*scale_factor);
            }
			feta.pop_back();//This is probably a BUG, the last element will be read twice if not using this statement
            functions[_name]->setFeta(feta);
			//functions[_name]->getHyperperiod();
        }
    }
    
    feta_source.close();
}

std::vector<Task* > * Parser::getRMTS()
{
    std::map<long, std::vector<Function *>* > tmp_tasks;
    
    // Scann all components
    for (auto i = components.begin(); i != components.end(); i++) {
        
        // Scann all functions in each component
        for (auto j = (*i)->getFunctions().begin();
                            j != (*i)->getFunctions().end(); j++) {
            //(*j).second->print(std::cout);
            auto k = tmp_tasks.find((*j).second->getPeriod());
            if (k != tmp_tasks.end())
                k->second->push_back((*j).second);
            else {
                std::vector<Function* >* tmp0= new std::vector<Function *>;
                tmp0->push_back((*j).second);
                tmp_tasks[(*j).second->getPeriod()] = tmp0;
            }
        }
    }
    
    // Create Tasks
    auto p = 1;
    for (auto i = tmp_tasks.begin(); i != tmp_tasks.end(); i++) {
        std::string tmp_name;
        
        // Create the name of the task as sum of the name of its functions
        for (auto j = (*i).second->begin(); j != (*i).second->end(); j++)
            tmp_name+= (*j)->getName();
        auto pr = p++;
        Task* tmp = new Task(tmp_name, pr, *(*i).second);
        tasks->push_back(tmp);
    }
    return tasks;
}

void Parser::checkFunctions()
{
    for (auto f : functions)
    {
        auto function = f.second;
        auto feta = function->get();
        
        auto startvalue = feta[0];
        for (auto i : feta)
        {
            if (i > startvalue) {
                std::cout << "ERROR: function " << function->getName()
                << " has a bad FETA" << std::endl;
                exit(-1);
            }
        }
    }
}

std::vector<Function*> Parser::getFunctions()
{
    std::vector<Function*> funs;
    for (auto f : functions)
        funs.push_back(f.second);
    return funs;
}

std::vector<Task* > * Parser::singleFunctionTask()
{
	auto p = 1;
	for (auto i = components.begin();i != components.end();i++)
	{
		for(auto j = (*i)->getFunctions().begin();j!= (*i)->getFunctions().end();j++)
		{
			std::string tmp_name = (*j).second->getName();
			std::vector<Function* > tmp_function;
			tmp_function.push_back((*j).second);
			Task *tmp = new Task(tmp_name, p, tmp_function);
			tasks->push_back(tmp);
			p++;
		}
	}
    
    return tasks;
}



