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

#include <cv.h>
#include <highgui.h>

#include "utils/helper.h"
#include "test/Test.h"
#include "Descriptors/LogPolarDescriptor.h"


using namespace std;

void buildSSM(){
	cout << "!!!Running!!!" << endl; // prints !!!Hello World!!!
	DataSet dataset;
	BackgroundModel bModel;
//	bModel.buildAllBackgroundsModel();return 0;
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
}

void buildSSMDescriptor(){
	system("rm -r " OUTPUT_DIR "LPD");
	system("mkdir -p " OUTPUT_DIR "LPD");

	string type = "Sil";
	DataSet dataset;
	for(int i=0;i<dataset.actions.size();i++){
		string action_code = dataset.actions.at(i);
		char* input_dir = (char*) malloc(200);
		sprintf(input_dir, OUTPUT_DIR "SSM/%s/%s/",type.c_str(),action_code.c_str());
		string output_file = string(OUTPUT_DIR "LPD/"+type+"_"+action_code+".txt");


		vector<string> files = fileList(input_dir,"png");
		for(int j=0;j<files.size();j++){
			int pos = files[i].rfind("/");
			string file_name = files[j].substr(pos+1, files[j].length()-pos-5);
			cout << "["<< file_name << "]" << endl;
			IPPR::LogPolarDescriptor descriptor;
			cv::Mat image = cv::imread(files[j]);
			cv::Mat grayImage;

			cvtColor(image, grayImage, CV_BGR2GRAY);


			descriptor.buildDescriptor(grayImage,50);
			descriptor.saveDescriptor(output_file,file_name);
		}
		free(input_dir);
	}

}

void clean();
int main() {
	clean();
	//	IPPR::Test test;test.run();return 0;
//	buildSSM();
	buildSSMDescriptor();
//	dirList("/home/mohammad/AUT/Project/IXMAS/Output/SSM/Sil/1/");
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

void clean(){
#ifdef CLEAN

#endif

}
