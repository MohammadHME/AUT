/*
 * VideoProccessor.cpp
 *
 *  Created on: May 7, 2012
 *      Author: Sayyed Mohammad Hashemi
 */

#include "stdafx.h"
#include "VideoProccessor.h"
#include "BackgroundModel.h"
#include "KLTTracker.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/contrib/contrib.hpp> // for colormap

#include <opencv/cv.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <sys/stat.h>
#include <string>

#include "cvblobs/BlobResult.h"
#include "cvblobs/BlobOperators.h"


using namespace std;
using namespace cv;

VideoProccessor::VideoProccessor() {
	file_index=0;
}

VideoProccessor::~VideoProccessor() {
}

Mat_<uchar> VideoProccessor::removeBackground(Mat &frame, Mat_<Vec3f> &bMean,
	Mat_<Vec3f> &bVar, Mat_<Vec3f> &bStd) {
	Mat_<Vec3b> frame_clone = frame.clone();

	cv::Size s(3, 3);
//	blur(frame, frame, s);
	Mat_<Vec3f> hsvFrame = frame.clone();
#ifdef HSV_BACKGROUND_SUBTRACTION
	cvtColor(hsvFrame, hsvFrame, CV_BGR2HSV);
#endif

	Mat_<uchar> forground, tempImage;
	forground.create(frame.rows, frame.cols);
	tempImage.create(frame.rows, frame.cols);
	//forground.zeros(frame.rows,frame.cols);

	for (int i = 0; i < frame.rows; i++){
		for (int j = 0; j < frame.cols; j++) {
			Vec3f f = hsvFrame(i, j);
			Vec3f m = bMean(i, j);
			Vec3f v = bVar(i, j);
			Vec3f std = bStd(i,j);
			int min=2;
			std[0] = std[0]<min?min:std[0];
			std[1] = std[1]<min?min:std[1];
			std[2] = std[2]<min?min:std[2];
			float d_b = abs(f[0] - m[0]);
			float d_g = abs(f[1] - m[1]);
			float d_r = abs(f[2] - m[2]);
			float c=3;

//			tempImage(i,j) = std[2]/180*256;
#ifdef HSV_BACKGROUND_SUBTRACTION
			float c1=0.05, c2=0.3;
			d_b = d_b<180?d_b:360-d_b;
			if (d_b < std[0]*c1 && d_g < std[1]*c2)
				forground(i, j) = 0;
			else
				forground(i, j) = 255;
#else
			if (d_b <= std[0]*c && d_g <= std[1]*c && d_r <= std[2]*c)
				forground(i, j) = 0;
			else{
				forground(i, j) = 255;
//				cout << i <<"," << j << "\t"<< d_b << "\t"<< d_g << "\t" << d_r << endl;
			}
#endif
		}
	}
#ifdef BKMORPH
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(7, 7));
	erode(forground, forground, kernel);
	dilate(forground, forground, kernel);
	dilate(forground, forground, kernel);
	erode(forground, forground, kernel);
#endif

#ifdef BBLOB
	return findBiggestBlob(forground);
//	CBlobResult blobs;
//	CBlob biggestBlob;
//
//
//	IplImage forgroundImage = forground;
//	IplImage *blobMask;
//	blobMask = cvCreateImage(cvGetSize(&forgroundImage), IPL_DEPTH_8U,3);
//	cvZero(blobMask);
//	blobs = CBlobResult(&forgroundImage, NULL, 0);
//	blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );
//	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, 100);
//	biggestBlob.FillBlob( blobMask, CV_RGB(255,255,255));
//
//	Mat_<Vec3b> temp = Mat(blobMask,false);
////	bitwise_and(frame_clone, temp, frame);
//	cvtColor(temp,forground,CV_BGR2GRAY);
//	return forground;


#else
	return forground;
#endif
	imwrite("/home/mohammad/AUT/Project/IXMAS/Output/std_h.bmp",tempImage);
}

void VideoProccessor::removeShadow(Mat &frame,Mat_<uchar> &forground, Mat_<Vec3f> &bMean_hsv,
	Mat_<Vec3f> &bVar_hsv, Mat_<Vec3f> &bStd_hsv){


	int i_min=forground.rows,i_max=0,j_min=forground.cols,j_max=0;

	for (int i=0;i<forground.rows-1;i++)
		for (int j=0;j<forground.cols-1;j++){
			if(forground(i,j)==0)
				continue;
		if(i<i_min)
			i_min = i;
		if(i>i_max)
			i_max=i;
		if(j<j_min)
			j_min = j;
		if(j>j_max)
			j_max=j;
		}
	Rect roi(j_min,i_min,j_max-j_min,i_max-i_min);
	Mat_<uchar> forgroundROI = forground(roi);

	Mat_<Vec3f> hsvFrame = frame(roi).clone();
	cvtColor(hsvFrame, hsvFrame, CV_BGR2HSV);
	Mat_<Vec3f> hsvbMean = bMean_hsv(roi);
	Mat_<Vec3f> hsvbVar = bVar_hsv(roi);
	Mat_<Vec3f> hsvbStd = bStd_hsv(roi);
	for (int i = 0; i < hsvFrame.rows; i++){
		for (int j = 0; j < hsvFrame.cols; j++) {
			Vec3f f = hsvFrame(i, j);
			Vec3f m = hsvbMean(i, j);
			Vec3f v = hsvbVar(i, j);
			Vec3f std = hsvbStd(i,j);
			float d_h = abs(f[0] - m[0]);
			float d_s = abs(f[1] - m[1]);
			float d_v = abs(f[2] - m[2]);

			float c1=0.5, c2=0.1;
			d_h = d_h<180?d_h:360-d_h;
			if(forgroundROI(i,j)==0)
				continue;
			if (d_h < std[0]*c1 && d_s < std[1]*c2)
				forgroundROI(i, j) = 0;
			else
				forgroundROI(i, j) = 255;
		}
	}

#ifdef BKMORPH
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	erode(forground, forground, kernel);
	dilate(forground, forground, kernel);
	dilate(forground, forground, kernel);
//	erode(forground, forground, kernel);
#endif
#ifdef BBLOB
	forground = findBiggestBlob(forground);
#endif
}

Mat_<uchar> VideoProccessor::findBiggestBlob(Mat_<uchar> &forground){
	CBlobResult blobs;
	CBlob biggestBlob;
	Mat_<uchar> result = forground.clone();

	IplImage forgroundImage = result;
	IplImage *blobMask;
	blobMask = cvCreateImage(cvGetSize(&forgroundImage), IPL_DEPTH_8U,3);
	cvZero(blobMask);
	blobs = CBlobResult(&forgroundImage, NULL, 0);
	blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, 100);
	biggestBlob.FillBlob( blobMask, CV_RGB(255,255,255));

	Mat_<Vec3b> temp = Mat(blobMask,false);
//	bitwise_and(frame_clone, temp, frame);
	cvtColor(temp,result,CV_BGR2GRAY);
	return result;
}

Mat_<Vec3f> VideoProccessor::nextFrame(string actor,string camera,int frameIndex) {
	// Reading next frame
	char* filename = (char*) malloc(200);
	sprintf(filename, DATASET_DIR "%s_png/cam%s/img%04d.png",
			actor.c_str() , camera.c_str(), frameIndex);
	Mat frame = imread(filename);
	free(filename);

#ifdef BKSUB
	// converting background image from RGB to HSV.
	// performance could be improved by moving this part to BackgroundModel class.
	int camera_code = atoi(camera.c_str());
	Mat result;
	frame.copyTo(result);
#ifdef HSV_BACKGROUND_SUBTRACTION
	removeBackground(result, bModel.bMean_hsv[camera_code],
			bModel.bVar_hsv[camera_code],bModel.bStd_hsv[camera_code]);
#else
	Mat_<uchar> forgroundMask = removeBackground(result, bModel.bMean[camera_code],
			bModel.bVar[camera_code],bModel.bStd[camera_code]);
	removeShadow(result,forgroundMask, bModel.bMean_hsv[camera_code],
			bModel.bVar_hsv[camera_code],bModel.bStd_hsv[camera_code]);
	cvtColor(forgroundMask,result,CV_GRAY2BGR);
#endif
	return result;
#else
	return frame;
#endif
}

void VideoProccessor::run(string actor,string action, string camera,int startFrame,int endFrame) {
	int frameIndex = startFrame;
	Mat frame;
	Mat_<Vec3f> output;
	Mat_<Vec3f> silhouette;
	 // KEY LINE: Start the window thread
	cvStartWindowThread();
	namedWindow("Frame",0);
	cvMoveWindow("Frame", 800, 300);
	KLTTracker tracker;
	tracker.begin();

	double total_time=0;
	while (frameIndex < endFrame) {
		clock_t start = clock();

		frame = nextFrame(actor,camera,frameIndex);

		vector<Point2f>  featurePoints;
#ifdef KLT
		tracker.process(frame);
#endif

		total_time += ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
		displayFrame(frame, actor,action,camera, (int)(frameIndex-startFrame)/total_time);

		char c = cvWaitKey(33);
		if (c == 27)
			exit(0);
		if(c==32){
			do{
				c = cvWaitKey();
			}while(c!=32);
		}
		frameIndex++;
	}
#ifdef KLT
	tracker.calculateSSMPos();
	tracker.drawSSMPos();
	tracker.drawSSMVel();
	saveMatrix(tracker.getSSMPos(),"Pos",actor,action,camera,startFrame,endFrame);
#endif
}

void VideoProccessor::saveMatrix(Mat_<uchar> matrix,string type, string actor,string action,
			string camera,int startFrame,int endFrame){


#if defined _WIN32 || defined _WIN64
//    _mkdir(strPath.c_str());
#elif defined(__linux__)
	char* mkdirCommand = (char*) malloc(200);
	sprintf(mkdirCommand, "mkdir -p "OUTPUT_DIR "%s/%s/",type.c_str(),action.c_str());
	system(mkdirCommand);
	free(mkdirCommand);

    char* filePath = (char*) malloc(200);
    sprintf(filePath, OUTPUT_DIR "%s/%s/%s_cam%s_%d_%d.png",
			type.c_str(),action.c_str(), actor.c_str(), camera.c_str(),startFrame,endFrame);

    char* cfilePath = (char*) malloc(200);
	sprintf(cfilePath, OUTPUT_DIR "%s/%s/c_%s_cam%s_%d_%d.png",
			type.c_str(),action.c_str(), actor.c_str(), camera.c_str(),startFrame,endFrame);
#endif
	imwrite(filePath,matrix);

	Mat colorFrame;
	resize(matrix,matrix,Size(500,500));
	applyColorMap(matrix,colorFrame,COLORMAP_JET);
	imwrite(cfilePath,colorFrame);

	free(filePath);
}

void VideoProccessor::displayFrame(Mat &frame, string actor, string action, string camera, int fps){
	stringstream info, ss;
	info << "Actor:" << actor;
	info << ", Action:" << action;
	info << ", Camera:" << camera;
	ss << "fps: " << fps;

	putText(frame, info.str(), cvPoint(10, 20), FONT_HERSHEY_COMPLEX_SMALL,
			0.8, cvScalar(255, 255, 255), 1, CV_AA);

	putText(frame, ss.str(), cvPoint(10, 40), FONT_HERSHEY_COMPLEX_SMALL,
				0.8, cvScalar(255, 255, 255), 1, CV_AA);
	imshow("Frame", frame);
}
void VideoProccessor::displayFrame(Mat &frame, int fps){
	stringstream ss;
	ss << "fps: ";
	ss << fps;

	putText(frame, ss.str(), cvPoint(10, 20), FONT_HERSHEY_COMPLEX_SMALL,
			0.8, cvScalar(255, 255, 255), 1, CV_AA);
	imshow("Frame", frame);
}





Mat_<Vec3f> VideoProccessor::nextFrame(int camera) {
	// Reading next frame
	char* filename = (char*) malloc(200);
	sprintf(filename, DATASET_DIR ACTOR "_png/cam%d/img%04d.png", camera, file_index);
	Mat frame = imread(filename);
	free(filename);
	file_index++;

#ifdef BKSUB
	// converting background image from RGB to HSV.
	// performance could be improved by moving this part to BackgroundModel class.
	Mat_<Vec3f> bMean = bModel.bMean[camera];
	Mat_<Vec3f> bVar = bModel.bVar[camera];
	Mat_<Vec3f> hsvbMean = bMean.clone();
	cvtColor(hsvbMean, hsvbMean, CV_BGR2HSV);
	Mat_<Vec3f> hsvbVar = bVar.clone();
	cvtColor(hsvbVar, hsvbVar, CV_BGR2HSV);
	Mat_<Vec3f> hsvbStd = bModel.bStd[camera].clone();
	cvtColor(hsvbStd, hsvbStd, CV_BGR2HSV);

	Mat_<Vec3f> result;
	frame.copyTo(result);

	removeBackground(result, hsvbMean, hsvbVar,hsvbStd);

	return result;
#else
	return frame;
#endif
}

void VideoProccessor::run(int camera) {
	file_index = 44;
	VideoCapture capture(0);
	Mat frame;
	Mat_<Vec3f> output;
	Mat_<Vec3f> silhouette;
	 // KEY LINE: Start the window thread
	cvStartWindowThread();
	namedWindow("Frame",0);
	cvMoveWindow("Frame", 800, 300);
//	FeatureExctractor extractor;
	KLTTracker tracker;
	tracker.begin();

	double total_time=0;
	while (file_index < 110) {
		//if (!capture.read(frame))
		//    break;
		clock_t start = clock();

		frame = nextFrame(camera);

		vector<Point2f>  featurePoints;
#ifdef KLT
		tracker.process(frame);
		//extractor.process(frame, silhouette, output);
#endif

		total_time += ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
		displayFrame(frame, (int)(file_index/total_time));

		char c = cvWaitKey(33);
		if (c == 27)
			break;
		if(c==32){
			do{
				c = cvWaitKey();
			}while(c!=32);
		}
	}
	tracker.calculateSSMPos();
	tracker.drawSSMPos();
	tracker.drawSSMVel();

}
