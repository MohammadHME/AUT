/*
 * helper.cpp
 *
 *  Created on: Jan 28, 2013
 *      Author: mohammad
 */
#include "helper.h"

#include <cv.h>
#include <highgui.h>
#include <dirent.h>

vector<string> fileList(string dir,string ext){
	vector<string> files;
	DIR *dp;
	struct dirent *ep;

	dp = opendir(dir.c_str());
	if(dp!=NULL){
		while (ep=readdir(dp)){
//			if(strcmp(ep->d_name,"..")==0 || strcmp(ep->d_name,".")==0)
//				continue;
			if(string(ep->d_name).find(ext)==-1)
				continue;
			string file = dir;
			file += string(ep->d_name);
			files.push_back(file);
			cout << file << endl;
		}
		(void) closedir(dp);
	}else
		perror("Couldn.t open the directory");
	return files;

}

void displayFrame(Mat frame){
	cvStartWindowThread();
	namedWindow("dFrame",0);
	cvMoveWindow("dFrame", 800, 300);
	imshow("dFrame", frame);
	cvWaitKey();
}



