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

#include "cvblobs/BlobResult.h"
#include "cvblobs/BlobOperators.h"


using namespace std;
using namespace cv;

VideoProccessor::VideoProccessor() {
	file_index=0;
}

VideoProccessor::~VideoProccessor() {
}

void VideoProccessor::removeBackground(Mat &frame, Mat_<Vec3f> &bMean,
	Mat_<Vec3f> &bVar) {
	Mat_<Vec3b> frame_clone = frame.clone();

	cv::Size s(3, 3);
//	blur(frame, frame, s);
	Mat_<Vec3f> hsvFrame = frame.clone();
	cvtColor(hsvFrame, hsvFrame, CV_BGR2HSV);

	Mat_<uchar> forground;
	forground.create(frame.rows, frame.cols);
	//forground.zeros(frame.rows,frame.cols);

	for (int i = 0; i < frame.rows; i++)
		for (int j = 0; j < frame.cols; j++) {
			Vec3f f = hsvFrame(i, j);
			Vec3f m = bMean(i, j);
			Vec3f v = bVar(i, j);
			float d_h = abs(f[0] - m[0]);
			float d_s = abs(f[1] - m[1]);
			float d_v = abs(f[2] - m[2]);
			if ((d_h < v[0]/5 || d_h > 360 - v[0]/5) && d_s < v[1]/10  //&& d_v < v[2]*10
					) {
				forground(i, j) = 255;
			} else {
				forground(i, j) = 0;
			}
		}
#ifdef BKMORPH
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	dilate(forground, forground, kernel);
	erode(forground, forground, kernel);
	erode(forground, forground, kernel);
	dilate(forground, forground, kernel);
#endif

#ifdef BBLOB
	CBlobResult blobs;
	CBlob biggestBlob;


	IplImage forgroundImage = forground;
	IplImage *blobMask;
	blobMask = cvCreateImage(cvGetSize(&forgroundImage), IPL_DEPTH_8U,3);
	cvZero(blobMask);
	blobs = CBlobResult(&forgroundImage, NULL, 255);
	blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, 100);
	biggestBlob.FillBlob( blobMask, CV_RGB(255,255,255));

	Mat_<Vec3b> temp = Mat(blobMask,false);
	bitwise_and(frame_clone, temp, frame);
#else
	frame=forground;
#endif
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

	Mat_<Vec3f> result;
	frame.copyTo(result);

	removeBackground(result, hsvbMean, hsvbVar);

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
	}
	tracker.calculateSSMPos();
	tracker.drawSSMPos();
	tracker.drawSSMVel();

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
	Mat_<Vec3f> bMean = bModel.bMean[camera_code];
	Mat_<Vec3f> bVar = bModel.bVar[camera_code];
	Mat_<Vec3f> hsvbMean = bMean.clone();
	cvtColor(hsvbMean, hsvbMean, CV_BGR2HSV);
	Mat_<Vec3f> hsvbVar = bVar.clone();
	cvtColor(hsvbVar, hsvbVar, CV_BGR2HSV);

	Mat_<Vec3f> result;
	frame.copyTo(result);

	removeBackground(result, hsvbMean, hsvbVar);

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
		displayFrame(frame, (int)(frameIndex-startFrame)/total_time);

		char c = cvWaitKey(33);
		if (c == 27)
			exit(0);
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

void VideoProccessor::displayFrame(Mat &frame, int fps){
	stringstream ss;
	ss << "fps: ";
	ss << fps;

	putText(frame, ss.str(), cvPoint(10, 20), FONT_HERSHEY_COMPLEX_SMALL,
			0.8, cvScalar(255, 255, 255), 1, CV_AA);
	imshow("Frame", frame);
}
