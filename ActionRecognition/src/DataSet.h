/*
 * DataSet.h
 *
 *  Created on: Nov 21, 2012
 *      Author: mohammad
 */
#include <string>
#include <vector>
#include <map>

#ifndef DATASET_H_
#define DATASET_H_

class Action{
public:
	Action(){start=0;end=0;};
	std::string name;
	int start;
	int end;
};

class Actor{
public:
	Actor();
	Actor(std::string,std::string);
	std::string name;
	std::vector<Action> actions;
};

typedef std::pair <std::string, Action> Action_Pair;
typedef std::pair <std::string, Actor> Actor_Pair;

class DataSet {
public:
	DataSet();
	virtual ~DataSet();
	std::vector<std::string> actors;
	std::vector<std::string> actions;
	std::vector<std::string> cameras;
	std::map<std::string,Actor> actorsmap;
private:
	void loadInputs();
	void loadConfigs();
};

#endif /* DATASET_H_ */


