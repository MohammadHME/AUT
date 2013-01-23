/*
 * SilhouetteProcessor.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: mohammad
 */

#include "SilhouetteProcessor.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/contrib/contrib.hpp> // for colormap

#include "../stdafx.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>

using namespace cv;

namespace IPPR {

SilhouetteProcessor::SilhouetteProcessor() {
}

SilhouetteProcessor::~SilhouetteProcessor() {
}


void SilhouetteProcessor::begin(){
	silhouettes.empty();
}

void SilhouetteProcessor::process(Mat &frame, Mat_<uchar> mask){
	silhouettes.push_back(mask);
}

void SilhouetteProcessor::calculateSSM(){
	Mat_<double> ssmFrame(silhouettes.size(),silhouettes.size());
	int max_element=1;
	for(int i=0;i<silhouettes.size();i++){
		for(int j=0;j<=i;j++){
			Mat_<uchar> dist_frame;
			subtract(silhouettes[i],silhouettes[j],dist_frame);
			int s = countNonZero(dist_frame);
			ssmFrame[i][j] = s;
			ssmFrame[j][i] = s;
			if(s>max_element)
				max_element = s;
		}
	}
	Mat_<uchar> result(silhouettes.size(),silhouettes.size());
	for(int i=0;i<silhouettes.size();i++){
		for(int j=0;j<silhouettes.size();j++){
			result(i,j) = (uchar)(ssmFrame(i,j)/max_element*255);
		}
	}
	_SSM = result;
}

void SilhouetteProcessor::drawSSM(){
	drawSSM(_SSM);
}

void SilhouetteProcessor::drawSSM(Mat_<uchar> grayFrame){
	resize(grayFrame,grayFrame,Size(500,500));
	Mat colorFrame;
	applyColorMap(grayFrame,colorFrame,COLORMAP_JET);

	imshow("Frame", colorFrame);
//		grayFrame.deallocate();
//	colorFrame.deallocate();
//	cvResizeWindow("Frame", 800, 800);
//	cvWaitKey(0);
}

} /* namespace IPPR */
