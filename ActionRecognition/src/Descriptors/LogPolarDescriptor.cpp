/*
 * LogPolarDescriptor.cpp
 *
 *  Created on: Jan 26, 2013
 *      Author: mohammad
 */

#include "LogPolarDescriptor.h"

#include <fstream>
#include <highgui.h>
#include "../utils/helper.h"

using namespace std;
using namespace cv;

namespace IPPR {

LogPolarDescriptor::LogPolarDescriptor() {

}

LogPolarDescriptor::~LogPolarDescriptor() {

}

void LogPolarDescriptor::buildDescriptor(cv::Mat image, int radius){
	int size = image.rows;
	//	GaussianBlur( grayFrame, grayFrame, Size(3,3), 0, 0, BORDER_DEFAULT );
	Mat grad_x, grad_y;
	Sobel( image, grad_x, CV_32F, 1, 0, 3);
	Sobel( image, grad_y, CV_32F, 0, 1, 3);

	Mat grad_mag, grad_angle;
	magnitude(grad_x,grad_y,grad_mag);
	phase(grad_x,grad_y,grad_angle);
	//	cartToPolar(grad_x,grad_y,grad_mag,grad_angle);

	double minVal,maxVal;
	minMaxLoc(grad_mag,&minVal,&maxVal);
	grad_mag*=(1./maxVal);
	grad_angle*=(1./3.14);
//	displayFrame(grad_angle);
	minMaxLoc(grad_mag,&minVal,&maxVal);
	Mat bigImage = Mat::zeros(size+2*radius+2,size+2*radius+2,CV_32F);
	Rect roi(radius,radius,size,size);
	grad_angle.copyTo(bigImage(roi));

	descriptor.empty();

	for(int i=radius;i<size+radius;i++){
		cout << i << " ";
		vector<float> localDes = buildLocalLogPolarDescriptor(bigImage,Point2d(i,i),radius);
		descriptor.push_back(localDes);
	}
	cout << endl;
}

vector<float> LogPolarDescriptor::buildLocalLogPolarDescriptor(cv::Mat image, cv::Point2d center, int radius){
	float r3=radius;
	float r2=r3/2;
	float r1=r2/2;
	float angles[] = {45, 22.5, -22.5, -67.5,-112.5,-135};
	int width=image.cols, height = image.rows;

	vector<float> ssd;
	ssd.empty();

	Mat_<uchar> mask(height,width);
	int index=0;

	mask = mask.zeros(height,width);
	ellipse(mask,center,Size(r1,r1),0,45,-135,Scalar(255),-1);
	vector<float> h = hist(image,mask, 8);
	ssd.insert(ssd.end(),h.begin(), h.end());
	index++;

	for(int i=0;i<5;i++){
		mask = mask.zeros(height,width);
		ellipse(mask,center,Size(r2,r2),0,angles[i],angles[i+1],Scalar(255),-1);
		ellipse(mask,center,Size(r1,r1),0,angles[i],angles[i+1],Scalar(0),-1);

		h = hist(image,mask, 8);
		ssd.insert(ssd.end(),h.begin(), h.end());

		index++;
	}

	for(int i=0;i<5;i++){
		mask = mask.zeros(height,width);
		ellipse(mask,center,Size(r3,r3),0,angles[i],angles[i+1],Scalar(255),-1);
		ellipse(mask,center,Size(r2,r2),0,angles[i],angles[i+1],Scalar(0),-1);

		h = hist(image,mask, 8);
		ssd.insert(ssd.end(),h.begin(), h.end());

		index++;
	}
	return ssd;
}

void LogPolarDescriptor::saveDescriptor(string outputfile, string name) {
	ofstream output;
	output.open(outputfile.c_str(),ios::out|ios::app);
	if (!output.is_open()){
		cout << "File Open Exception: " << outputfile << endl;
		return;
	}

	output << name << " {";
	for(int i=0;i<descriptor.size();i++){
		vector<float> localDes = descriptor[i];
		output <<" [";
		for(int j=0;j<localDes.size();j++)
			output << localDes[j] <<", ";
		output << "];";
	}
	output << " }" << endl;
	output.close();
}

void LogPolarDescriptor::showDescriptor() {
	cout << "Log Polar Descriptor:" << endl;
	for(int i=0;i<descriptor.size();i++){
		vector<float> localDes = descriptor[i];
		cout << "[";
		for(int j=0;j<localDes.size();j++)
			cout << localDes[j] <<"; ";
		cout << "]" << endl;
	}
}

vector<float> LogPolarDescriptor::hist(Mat frame, Mat_<uchar> mask, int nbins){
	MatND hist;
	int channels[] = {0};
	int histSize[] = {nbins};
	double minVal,maxVal;
	minMaxLoc(frame,&minVal,&maxVal);
	float range[] = { 0, 1 } ; //the upper boundary is exclusive
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
