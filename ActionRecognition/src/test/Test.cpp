/*
 * Test.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: mohammad
 */

#include "Test.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>

using namespace cv;

namespace IPPR {

Test::Test() {
}

Test::~Test() {
}

void Test::run(){
	logPolarMask();
}

void Test::logPolarMask(){
	Mat frame = imread("/home/mohammad/AUT/Project/IXMAS/Output/Sil/1/c_alba1_cam0_53_97.png");
	Mat grayFrame;
	cvtColor(frame, grayFrame, CV_BGR2GRAY);
	int width=grayFrame.cols, height = grayFrame.rows;
	Mat_<uchar> mask(height,width);
	mask.zeros(height,width);

//	circle(mask,Point(200,200),100,Scalar(255,255,255),-1,20);
	ellipse(mask,Point(200,200),Size(150,150),0,45,-30,Scalar(255,255,255),-1);
	ellipse(mask,Point(200,200),Size(50,50),0,45,-30,Scalar(0,0,0),-1);
//	line(mask,Point(0,0),Point(height,width),Scalar(255,255,255));

	Mat_<uchar> temp;
	bitwise_and(grayFrame, mask, grayFrame);

	int channels[] = {0};
	Mat hist;

	int histSize[] = {8};
	float range[] = { 0, 256 } ; //the upper boundary is exclusive
	const float* ranges = { range };
	bool uniform = true; bool accumulate = false;

//	 calcHist( &grayFrame, 1, channels, Mat(), // do not use mask
//	 hist, 1, histSize, ranges,
//	 uniform, // the histogram is uniform
//	 accumulate );


	cvStartWindowThread();
	namedWindow("Frame",0);
	cvMoveWindow("Frame", 800, 300);
	imshow("Frame", grayFrame);
	cvWaitKey();
}

} /* namespace IPPR */
