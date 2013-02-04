/*
 * Test.cpp
 *
 *  Created on: Jan 7, 2013
 *      Author: mohammad
 */

#include "Test.h"

#include <cv.h>
#include <highgui.h>
#include "../Descriptors/LogPolarDescriptor.h"
#include <dirent.h>

#include <iostream>

using namespace cv;
using namespace std;

namespace IPPR {

Test::Test() {

}

Test::~Test() {
}


void Test::run(){
	cvStartWindowThread();
	namedWindow("Frame",0);
	cvMoveWindow("Frame", 800, 300);


	LogPolarDescriptor des;
	Mat image = imread("/home/mohammad/Pictures/DataSet1/peppers.tif");
	Mat grayFrame;
	cvtColor(image, grayFrame, CV_BGR2GRAY);
	des.buildDescriptor(grayFrame,100);
	des.showDescriptor();


//	Mat grad_x, grad_y;
//	Sobel( grayFrame, grad_x, CV_32F, 1, 0, 3);
//	Sobel( grayFrame, grad_y, CV_32F, 0, 1, 3);
//
//	Mat grad_mag, grad_angle;
//	magnitude(grad_x,grad_y,grad_mag);
//	phase(grad_x,grad_y,grad_angle);
//
//	double minVal,maxVal;
//	minMaxLoc(grad_mag,&minVal,&maxVal);
//	grad_mag*=(1./maxVal);
//	des.buildLocalLogPolarDescriptor(grad_angle,Point2d(100,100),100);

//	Mat bigImage = Mat::zeros(712,712,CV_8U);
//	Rect roi(100,100,512,512);
//	grayFrame.copyTo(bigImage(roi));
//
//	cvStartWindowThread();
//	namedWindow("Frame",0);
//	cvMoveWindow("Frame", 800, 300);
//	imshow("Frame", bigImage);
//	cvWaitKey();


//	Point2d center(200,200);
//	logPolarMask(center,100);
}

void Test::logPolarMask(Point2d center, int radius){
//	float base = pow(radius,(float)1/3);
//	float r1 = base;
//	float r2 = pow(base,2);
//	float r3 = radius;

	float r3=radius;
	float r2=r3/2;
	float r1=r2/2;

	cvStartWindowThread();
	namedWindow("Frame",0);
	cvMoveWindow("Frame", 800, 300);


	float angles[] = {45, 22.5, -22.5, -67.5,-112.5,-135};
//	Mat frame = imread("/home/mohammad/AUT/Project/IXMAS/Output/Pos/1/c_alba1_cam0_53_97.png");
	Mat frame = imread("/home/mohammad/Pictures/DataSet2/4.2.04.tiff");

	Mat grayFrame;
	cvtColor(frame, grayFrame, CV_BGR2GRAY);
	int width=grayFrame.cols, height = grayFrame.rows;

//	GaussianBlur( grayFrame, grayFrame, Size(3,3), 0, 0, BORDER_DEFAULT );

	Mat grad_x, grad_y;
	Sobel( grayFrame, grad_x, CV_32F, 1, 0, 3);
	Sobel( grayFrame, grad_y, CV_32F, 0, 1, 3);

//	Mat abs_grad_x, abs_grad_y;
//	convertScaleAbs( grad_x, abs_grad_x );
//	convertScaleAbs( grad_y, abs_grad_y );

	Mat grad_mag, grad_angle;
//	magnitude(grad_x,grad_y,grad_mag);
	phase(grad_x,grad_y,grad_angle);
//	cartToPolar(grad_x,grad_y,grad_mag,grad_angle);

	multiply(grad_x,grad_x,grad_x);
	multiply(grad_y,grad_y,grad_y);
	Mat grad_sum;
	add(grad_x,grad_y,grad_sum);
	sqrt(grad_sum,grad_mag);
//	addWeighted(grad_x,0.5,grad_y,0,5,grad_mag);

	double minVal,maxVal;
	minMaxLoc(grad_mag,&minVal,&maxVal);
	grad_mag*=(1./maxVal);
	imshow("Frame", grad_mag);
	cvWaitKey();
//	return;

	vector<float> ssd;ssd.empty();


	Mat_<uchar> mask(height,width);
	int index=0;

	mask = mask.zeros(height,width);
	ellipse(mask,center,Size(r1,r1),0,45,-135,Scalar(255),-1);
	vector<float> h = hist(grayFrame,mask, 8);
	ssd.insert(ssd.end(),h.begin(), h.end());
	index++;
	imshow("Frame", mask);
	cvWaitKey();

	for(int i=0;i<5;i++){
		mask = mask.zeros(height,width);
		ellipse(mask,center,Size(r2,r2),0,angles[i],angles[i+1],Scalar(255),-1);
		ellipse(mask,center,Size(r1,r1),0,angles[i],angles[i+1],Scalar(0),-1);

		h = hist(grayFrame,mask, 8);
		ssd.insert(ssd.end(),h.begin(), h.end());

		index++;
		imshow("Frame", mask);
		cvWaitKey();
	}

	for(int i=0;i<5;i++){
		mask = mask.zeros(height,width);
		ellipse(mask,center,Size(r3,r3),0,angles[i],angles[i+1],Scalar(255),-1);
		ellipse(mask,center,Size(r2,r2),0,angles[i],angles[i+1],Scalar(0),-1);

		h = hist(grayFrame,mask, 8);
		ssd.insert(ssd.end(),h.begin(), h.end());

		index++;
		imshow("Frame", mask);
		cvWaitKey();
	}

	for(int i=0;i<ssd.size();i++)
		cout << ssd[i] << "\n";


return;

//	Mat_<uchar> temp;
////	bitwise_and(grayFrame, mask, grayFrame);
//
//	int channels[] = {0};
//	MatND hist;
//
//	int histSize[] = {8};
//	float range[] = { 0, 256 } ; //the upper boundary is exclusive
//	const float* ranges[] = { range };
//	bool uniform = true; bool accumulate = false;
//
//	 calcHist(&grayFrame, 1, channels, mask, // do not use mask
//	 hist,
//	 1, // dims
//	 histSize,
//	 ranges,
//	 uniform, // the histogram is uniform
//	 accumulate
//	 );
//
//	 Mat histImage( 500, 500, CV_8UC3, Scalar( 0,0,0) );
//	 normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
//	 int nbins = histSize[0];
//	 for( int i = 1; i < nbins; i++ )
//	   {
//		 	 std::cout << hist.at<float>(i-1) << std::endl;
//		 	 line( histImage, Point( 500/nbins*(i-1), 500 - cvRound(hist.at<float>(i-1)) ) ,
//	                        Point( 500/nbins*(i), 500 - cvRound(hist.at<float>(i)) ),
//	                        Scalar( 255, 0, 0), 2, 8, 0  );
//	   }
//
//	cvStartWindowThread();
//	namedWindow("Frame",0);
//	cvMoveWindow("Frame", 800, 300);
//	imshow("Frame", grayFrame);
//	cvWaitKey();
}

vector<float> Test::hist(Mat frame, Mat_<uchar> mask, int nbins){
	MatND hist;
	int channels[] = {0};
	int histSize[] = {nbins};
	float range[] = { 0, 256 } ; //the upper boundary is exclusive
	const float* ranges[] = { range };
	bool uniform = true; bool accumulate = false;

	 calcHist(&frame, 1, channels, mask, // do not use mask
	 hist,
	 1, // dims
	 histSize,
	 ranges,
	 uniform, // the histogram is uniform
	 accumulate
	 );

	 normalize(hist, hist, 1);

	 vector<float> result(nbins);
	 for( int i = 0; i < nbins; i++ )
		 result[i] =  hist.at<float>(i);
 	 return result;
}

} /* namespace IPPR */
