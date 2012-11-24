//============================================================================
// Name        : ActionRecognition.cpp
// Author      : Sayyed Mohammad Hashemi
// Version     :
// Copyright   : 
// Description : Ansi-style
//============================================================================
#include "stdafx.h"
#include <iostream>
#include "BackgroundModel.h"
#include "VideoProccessor.h"
#include "DataSet.h"
#include <string>
#include <map>
using namespace std;

int main() {
	cout << "!!!Running!!!" << endl; // prints !!!Hello World!!!
	DataSet dataset;
	BackgroundModel bModel;
	bModel.loadBackgroundsModel();
	VideoProccessor vp;
	vp.bModel=bModel;
	//for each actors
	for(int i=0;i<dataset.actors.size();i++){
		std::string actor_name = dataset.actors.at(i);
		map<string,Actor>::iterator actor_it;
		actor_it = dataset.actorsmap.find(actor_name);
		if(actor_it!=dataset.actorsmap.end()){
			cout << actor_it->second.name << endl;
			//for each action
			for(int j=0;j<dataset.actions.size();j++){
				string action_code = dataset.actions.at(j);
				for(int k=0;k<actor_it->second.actions.size();k++){
					if(actor_it->second.actions.at(k).name.compare(action_code)!=0)
						continue;
					int startFrame = actor_it->second.actions.at(k).start;
					int endFrame = actor_it->second.actions.at(k).end;
					//for each camera
					for(int c=0;c<dataset.cameras.size();c++){
						string camera_name = dataset.cameras.at(c);
						cout << actor_name << " " << action_code << " "
								<< startFrame << " " << endFrame << " "
								<< camera_name << endl;
						vp.run(actor_name,action_code,
								camera_name,startFrame,endFrame);

					}
				}
			}
		}
	}
	return 0;

//	BackgroundModel bModel;
//
////	bModel.buildAllBackgroundsModel();
//
//	bModel.loadBackgroundsModel();
//	VideoProccessor vp;
//	vp.bModel=bModel;
//
//
//
//	vp.run(CAMERA);


	return 0;
}
