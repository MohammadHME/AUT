/*
 * OpticalFlowProcessor.cpp
 *
 *  Created on: Dec 31, 2012
 *      Author: mohammad
 */

#include "OpticalFlowProcessor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/contrib/contrib.hpp> // for colormap

#include "../stdafx.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "quiver.h"

using namespace cv;

namespace IPPR {

OpticalFlowProcessor::OpticalFlowProcessor() {

}

OpticalFlowProcessor::~OpticalFlowProcessor() {

}

void OpticalFlowProcessor::begin(){
	_flowVector.begin();
	_frameCounter=0;
}

void OpticalFlowProcessor::process(Mat &frame, Mat_<uchar> mask){
	Mat grayFrame;
	cvtColor(frame, grayFrame, CV_BGR2GRAY);
	if (_prevGrayFrame.empty())
	        grayFrame.copyTo(_prevGrayFrame);

	Mat flow = Mat(grayFrame.size(), CV_32FC2);
	double pyr_scale =0.5;
	int levels = 3;
	int winsize = 5;
	int iterations = 5;
	int poly_n = 5;
	double poly_sigma = 1.1;
	int flags = 0; //OPTFLOW_FARNEBACK_GAUSSIAN;
	calcOpticalFlowFarneback(_prevGrayFrame,grayFrame,flow,pyr_scale,levels,winsize,iterations,poly_n,poly_sigma,flags);

	_flowVector.push_back(flow);
	_frameCounter++;

	cvQuiver(flow, frame,10, CV_RGB(0,255,0));
	_prevGrayFrame = grayFrame;
//	drawOptFlowMap(flow, frame, 10, CV_RGB(0,255,0));

}

void OpticalFlowProcessor::calculateSSMOF(){
	Mat_<double> ssmOFFrame(_frameCounter,_frameCounter);
	double max_dist=0;
	for(int i=0;i<_frameCounter;i++){
		for(int j=0;j<=i;j++){
			Mat flow_i = _flowVector[i];
			Mat flow_j = _flowVector[j];
			Mat flow_dist;
			subtract(flow_i,flow_j,flow_dist);

			vector<Mat_<float> > planes;
			split( flow_dist, planes);
			Mat_<float> dist_x = planes[0];
			Mat_<float> dist_y = planes[1];

			pow(dist_x,2,dist_x);
			pow(dist_y,2,dist_y);

			Mat_<float> dist;


			add(dist_x,dist_y,dist);

			sqrt(dist,dist);

			double totalDist = sum(dist)[0];

			if(totalDist>max_dist)
				max_dist= totalDist;

			ssmOFFrame(i,j)=totalDist;
			ssmOFFrame(j,i)=ssmOFFrame(i,j);
		}
	}
	ssmOFFrame=ssmOFFrame/max_dist*255;
	ssmOFFrame.assignTo(_SSMOF,CV_8UC1);
}

void OpticalFlowProcessor::drawSSMOF(){
	drawSSM(_SSMOF);
}

cv::Mat_<uchar> OpticalFlowProcessor::getSSMOF(){
	return _SSMOF;
}

void OpticalFlowProcessor::drawSSM(Mat_<uchar> grayFrame){
	resize(grayFrame,grayFrame,Size(500,500));
	Mat colorFrame;
	applyColorMap(grayFrame,colorFrame,COLORMAP_JET);

	imshow("Frame", colorFrame);
//		grayFrame.deallocate();
//	colorFrame.deallocate();
//	cvResizeWindow("Frame", 800, 800);
//	cvWaitKey();
}

} /* namespace IPPR */
