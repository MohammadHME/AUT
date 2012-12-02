/*
 * BackgroundModel.cpp
 *
 *  Created on: May 4, 2012
 *      Author: mohammad
 */

#include "BackgroundModel.h"
#include "stdafx.h"
#include <cstdlib>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include <stdio.h>

#include <unistd.h>
using namespace std;
using namespace cv;

#define HSV

BackgroundModel::BackgroundModel() {

}

BackgroundModel::~BackgroundModel() {

}

void BackgroundModel::buildAllBackgroundsModel() {
	int framecounts=380;
	for(int i=0;i<5;i++){
		char* dir = (char*) malloc(200);
		sprintf(dir, DATASET_DIR BACKGROUND_DIR "cam%d/", i);
		buildBackgroundModel(dir, i, framecounts);
		free(dir);

	}
}

void BackgroundModel::saveBackgroundsModel() {
//	FileStorage fs(DATASET_DIR BACKGROUND_MODEL_DIR "background.yml", FileStorage::WRITE);
//	fs << "backgrounds" << "[";
//	for(int i=0;i<NUMBER_OF_CAMERAS;i++){
//		Mat_<Vec3b> m = bMean[i];
//		Mat_<Vec3b> v = bVar[i];
//		fs << "{:" << "mean" << m << "variance" << v <<"}";
//	}
//	fs << "]";
//	fs.release();
}

void BackgroundModel::loadBackgroundsModel() {
//	FileStorage fs(DATASET_DIR BACKGROUND_MODEL_DIR "background.yml", FileStorage::READ);
//	FileNode backgrounds = fs["backgrounds"];
//	FileNodeIterator it = backgrounds.begin(), it_end = backgrounds.end();
//	int idx = 0;
//	for( ; it != it_end; ++it, idx++ ){
//		(*it)["mean"] >> bMean[idx];
//		(*it)["variance"] >> bVar[idx];
//	}

	char* outputMeanFile = (char*) malloc(200);
	char* outputVarFile = (char*) malloc(200);
	char* outputStdFile = (char*) malloc(200);
	for(int i=0;i<NUMBER_OF_CAMERAS;i++){
		sprintf(outputMeanFile, DATASET_DIR BACKGROUND_MODEL_DIR "cam%d_mean.bmp", i);
		sprintf(outputVarFile, DATASET_DIR BACKGROUND_MODEL_DIR "cam%d_var.bmp", i);
		sprintf(outputStdFile, DATASET_DIR BACKGROUND_MODEL_DIR "cam%d_std.bmp", i);
		bMean[i]=imread(outputMeanFile);
		bVar[i]=imread(outputVarFile);
		bStd[i]=imread(outputStdFile);
		cvtColor(bMean[i],bMean_hsv[i],CV_BGR2HSV);
		cvtColor(bVar[i],bVar_hsv[i],CV_BGR2HSV);
		cvtColor(bStd[i],bStd_hsv[i],CV_BGR2HSV);
	}
	free(outputMeanFile);free(outputVarFile);

	namedWindow("Frame");
	cvMoveWindow("Frame", 800, 300);
//	for(int i=0;i<NUMBER_OF_CAMERAS;i++){
//		imshow("Frame",(Mat_<Vec3b>)bMean[i]);
//		cvWaitKey();
//	}

}

void BackgroundModel::buildBackgroundModel(char* dir, int camera, int framecounts){
	cout << "Building background model for cam " << camera << " ..."<< endl;
	char* frameFile = (char*) malloc(200);
	Mat_<Vec3f> sum, sum2;
#ifdef DEBUG
	    namedWindow("Frame");
	    cvMoveWindow("Frame", 800, 300);
#endif
	    cout << "Computing backgrounds mean..." << endl;
	    for (int index = 0; index < framecounts; index++) {
	        sprintf(frameFile, "%simg%04d.png", dir, index+1);
	        Mat_<Vec3f> frame = imread(frameFile);
	        resize(frame,frame,Size(390,291));
#ifdef BLUR_ENABLED
	    	cv::Size s(3, 3);
	    	blur(frame, frame, s);
#endif
#ifdef HSV
	        Mat_<Vec3f> hsvframe = frame.clone();
	        cvtColor(hsvframe, hsvframe, CV_BGR2HSV);
#endif
	        if(index==0){
	        	sum=frame;
#ifdef HSV
	        	sum2=hsvframe;
#endif
	        }else{
	        	sum = sum+frame;
#ifdef HSV
	        	sum2= sum2+hsvframe;
#endif
	        }
#ifdef DEBUG
	        stringstream ss;
	        ss << "Camera " << camera <<": Frame "<<index << " of " << framecounts << " Pass 1";
			putText(frame,ss.str(),cvPoint(10,20), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,0,50), 1, CV_AA);
	        imshow("Frame", (Mat_<Vec3b>)frame);
	        cvWaitKey(1);
#endif
	    }
	    cout << "Computing backgrounds variance..." << endl;
	    Mat_<Vec3f> mean = sum/framecounts;
	    Mat_<Vec3f> hsvmean = sum2/framecounts;
	    for (int index = 0; index < framecounts; index++) {
			sprintf(frameFile, "%simg%04d.png", dir, index+1);
			Mat_<Vec3f> frame = imread(frameFile);
			resize(frame,frame,Size(390,291));
#ifdef BLUR_ENABLED
	    	cv::Size s(3, 3);
	    	blur(frame, frame, s);
#endif
#ifdef HSV
			Mat_<Vec3f> hsvframe = frame.clone();
			cvtColor(hsvframe, hsvframe, CV_BGR2HSV);
#endif
			if(index==0){
				sum=(frame - mean).mul(frame - mean);
#ifdef HSV

				sum2=(hsvframe - hsvmean).mul(hsvframe - hsvmean);
#endif
			}else{
				sum = sum + (frame - mean).mul(frame - mean);
#ifdef HSV
				for (int i = 0; i < hsvframe.rows; i++){
					for (int j = 0; j < hsvframe.cols; j++) {
						Vec3f f = hsvframe(i, j);
						if(f[0]>180)
							f[0] = 360-f[0];
						hsvframe(i, j)=f[0];
					}
				}
				sum2 = sum2 + (hsvframe - hsvmean).mul(hsvframe - hsvmean);
#endif
			}
#ifdef DEBUG
			stringstream ss;
			ss << "Camera " << camera <<": Frame "<<index << " of " << framecounts << " Pass 2";
			putText(frame,ss.str(),cvPoint(10,20), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(255,0,50), 1, CV_AA);
			imshow("Frame", (Mat_<Vec3b>)frame);
			cvWaitKey(1);
#endif
		}
	    Mat_<Vec3f> variance = sum/(framecounts - 1-1);
	    Mat_<Vec3f> std;
	    cv::sqrt(variance,std);
#ifdef HSV
	    Mat_<Vec3f> hsv_variance = sum2/(framecounts - 1-1);
	    Mat_<Vec3f> hsv_std;
		cv::sqrt(hsv_variance,hsv_std);
#endif
	    bMean[camera] = mean;
	    bVar[camera] = variance;
	    bStd[camera] = std;

	    //Saving results as images.
	    char* outputMeanFile = (char*) malloc(200);
	    char* outputVarFile = (char*) malloc(200);
	    char* outputStdFile = (char*) malloc(200);
		sprintf(outputMeanFile, DATASET_DIR BACKGROUND_MODEL_DIR "cam%d_mean.bmp", camera);
		sprintf(outputVarFile, DATASET_DIR BACKGROUND_MODEL_DIR "cam%d_var.bmp", camera);
		sprintf(outputStdFile, DATASET_DIR BACKGROUND_MODEL_DIR "cam%d_std.bmp", camera);
		imwrite(outputMeanFile ,mean);
		imwrite(outputVarFile ,variance);
		imwrite(outputStdFile ,std);

#ifdef HSV
		sprintf(outputMeanFile, DATASET_DIR BACKGROUND_MODEL_DIR "cam%d_hsv_mean.bmp", camera);
		sprintf(outputVarFile, DATASET_DIR BACKGROUND_MODEL_DIR "cam%d_hsv_var.bmp", camera);
		char* outputStdFile0 = (char*) malloc(200);
		char* outputStdFile1 = (char*) malloc(200);
		char* outputStdFile2 = (char*) malloc(200);
		sprintf(outputStdFile0, DATASET_DIR BACKGROUND_MODEL_DIR "hsv/cam%d_hsv_std[0].bmp", camera);
		sprintf(outputStdFile1, DATASET_DIR BACKGROUND_MODEL_DIR "hsv/cam%d_hsv_std[1].bmp", camera);
		sprintf(outputStdFile2, DATASET_DIR BACKGROUND_MODEL_DIR "hsv/cam%d_hsv_std[2].bmp", camera);
//		imwrite(outputMeanFile ,hsvmean);
//		imwrite(outputVarFile ,hsv_variance);
		vector<Mat> hsv_planes;
		split( hsv_std, hsv_planes );
		imwrite(outputStdFile0 ,hsv_planes[0]/180*255);
		imwrite(outputStdFile1 ,hsv_planes[1]*255);
		imwrite(outputStdFile2 ,hsv_planes[2]/100*255);
		free(outputStdFile0);free(outputStdFile1);free(outputStdFile2);
#endif
	    cout << "Building background model for cam " << camera << " finished."<< endl;

	    free(frameFile);
		free(outputMeanFile);
		free(outputVarFile);
		free(outputStdFile);
}
