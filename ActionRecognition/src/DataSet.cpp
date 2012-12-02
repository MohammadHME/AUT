/*
 * DataSet.cpp
 *
 *  Created on: Nov 21, 2012
 *      Author: mohammad
 */
#include "stdafx.h"
#include "DataSet.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ext/hash_map>

using namespace std;

DataSet::DataSet() {
	loadInputs();
	loadConfigs();
}

DataSet::~DataSet() {
	// TODO Auto-generated destructor stub
}

void DataSet::loadInputs() {
	string input;
	input = "";
	ifstream infile(DATASET_INPUT_FILE);
	if (infile.is_open()) {
		while (infile.good()) {
			string line;
			infile >> line;
			if (line.compare("Cameras{") == 0) {
				infile >> line;
				while (line.compare("}") != 0) {
					cameras.push_back(line);
					infile >> line;
				}
			}
			if (line.compare("Actors{") == 0) {
				infile >> line;
				while (line.compare("}") != 0) {
					actors.push_back(line);
					infile >> line;
				}
			}
			if (line.compare("Actions{") == 0) {
				infile >> line;
				while (line.compare("}") != 0) {
					actions.push_back(line);
					infile >> line;
				}
			}
			input += line + "\n";
		}
		infile.close();
	} else
		cout << "Unable to open input file";
	cout << input;
}

void DataSet::loadConfigs() {
		string input;

		ifstream infile(DATASET_CONFIG_FILE);
		if (infile.is_open()) {
			while (infile.good()) {
				string line;
				input = "";
				getline(infile,line);
				int p=0;
				if((p=line.find("{"))!=-1){
					string name = line.substr(0,p);
					getline(infile,line);
					while (line.compare("}") != 0) {
						input+=line+"\n";
						getline(infile,line);
					}
					Actor actor(name,input);
					actorsmap.insert(Actor_Pair(name,actor));
				}
			}
			infile.close();
		} else
			cout << "Unable to open input file";
}

Actor::Actor(string name,string content){
	this->name=name;

	istringstream in;
	in.str(content);
	string temp;
	cout << "______" << name << "_______" <<endl;
	while(!in.eof()){
		Action action;
		in >> action.name >> action.start >> temp >> action.end;
		action.name = action.name.substr(0,action.name.length()-1);
		if(action.start==0 && action.end==0)
			continue;
		cout << action.name << "," << action.start << ","
				<< action.end << endl;
		actions.push_back(action);
	}
//	cout << "END";
//	cout << name << endl << content;
}




