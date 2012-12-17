/*
 * KLTTracker.cpp
 *
 *  Created on: May 11, 2012
 *      Author: Sayyed Mohammad Hashemi
 */

#include "KLTTracker.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/contrib/contrib.hpp> // for colormap

#include "stdafx.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>


using namespace cv;

KLTTracker::KLTTracker() {
	lastPointIndex=0;
	index=0;
	min_dist_btw_Points=5;
	qlevel = 0.01;
}

KLTTracker::~KLTTracker() {

}

void KLTTracker::begin() {
	index=0;
	alivePointsCounter=0;
	for(int i=0;i<featurePoints.size();i++)
		featurePoints[i].empty();
	featurePoints.empty();

	for (int i=0;i<MAX_ACTION_LENGTH;i++)
		for(int j=1;j<MAX_NUMBER_OF_POINTS_LEN;j++)
			isAlivePoint[i][j]=true;
}

void KLTTracker::process(cv::Mat &frame, cv::Mat_<uchar> mask) {
    Mat grayFrame;
    cvtColor(frame, grayFrame, CV_BGR2GRAY);


    if(index>0)
    	points[0] = featurePoints[index-1];

    if (prevGrayFrame.empty())
        grayFrame.copyTo(prevGrayFrame);

    std::vector<cv::Point2f>  newFeaturePoints;
//    if (index==0 || points[0].size() <= MAX_NUMBER_OF_POINTS_TO_TRACK) {
    if (alivePointsCounter < MAX_NUMBER_OF_POINTS_TO_TRACK){
        // detect the features
        cv::goodFeaturesToTrack(grayFrame, // the image
        		newFeaturePoints, // the output detected features
        		MAX_NUMBER_OF_POINTS_TO_TRACK - alivePointsCounter, // the maximum number of features
                qlevel, // quality level
                min_dist_btw_Points// min distance between two features
                ,mask);
        // add the detected features to the currently tracked features
        points[0].insert(points[0].end(),
        		newFeaturePoints.begin(), newFeaturePoints.end());
        initial.insert(initial.end(),
        		newFeaturePoints.begin(), newFeaturePoints.end());
        std::cout << newFeaturePoints.size() << "\t" << points[0].size() << "\n";
    }

    if(points[0].empty())
        return;
    double d = 1e-4;
    Size s(39,39);
    TermCriteria t(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01);
    calcOpticalFlowPyrLK(
    		prevGrayFrame, grayFrame, // 2 consecutive images
            points[0], // input point positions in first image
            points[1], // output point positions in the 2nd image
            status, // tracking success
            err// tracking error
//            );
           ,s,
           3,
		   t,
		   0,
		   d);
    // 2. loop over the tracked points to reject some
//    int k = 0;
#ifdef KLT_DEBUG
    std::cout << "Dead points: ";
#endif
    alivePointsCounter=0;
    for (int i = 0; i < points[1].size(); i++) {
        if (	status[i]
            	&& (index==0 || (index>0 && isAlivePoint[index-1][i]) || i>=lastPointIndex)
                // if point has moved
                && (pow(points[0][i].x - points[1][i].x,2) + pow(points[0][i].y - points[1][i].y,2) < 20)
                ) {
        	isAlivePoint[index][i]=true;
        	alivePointsCounter++;
//        	isAlivePoint[i]=true;
//            // keep this point in vector
//        	points[0][k] = points[0][i];
//        	initial[k] = initial[i];
//            points[1][k++] = points[1][i];

        }else{
#ifdef KLT_DEBUG
        	if(status[i])
        		std::cout << i << " ";
        	else
        		std::cout << i <<"# ";
#endif
        	isAlivePoint[index][i]=false;
//        	if(status[i]
//                  	&& (index==0 || (index>0 && isAlivePoint[index-1][i]) || i>=lastPointIndex)){
//            	std::cout <<i<<"\t"<< (int)(points[0][i]).x << "-" << points[1][i].x
//            			<< "," << points[0][i].y <<"-"<< points[1][i].y
//            			<< ":" << pow(points[0][i].x - points[1][i].x,2) + pow(points[0][i].y - points[1][i].y,2)
//            			<< "\n";
//        	}

        }
    }
#ifdef KLT_DEBUG
    std::cout << "\n";
#endif
    for (int i = points[1].size(); i<MAX_NUMBER_OF_POINTS_TO_TRACK; i++)
            	isAlivePoint[index][i]=false;
    lastPointIndex = points[1].size();
    // eliminate unsuccesful points
//    points[1].resize(k);
//    points[0].resize(k);
//    initial.resize(k);
//    featurePoints = points[1];

    featurePoints.push_back(points[1]);
    index++;
    drawTrackedPoints(frame,index,initial,points[1]);

    // 4. current points and image become previous ones
    std::swap(points[1], points[0]);
    cv::swap(prevGrayFrame, grayFrame);
}

void KLTTracker::calculateSSMPos(){
//	_SSMPos(index,index);
	Mat_<double> ssmPosFrame(index,index);
	Mat_<double> ssmVelFrame(index,index);
	int numberOfAlivePoints=0;
		for(int i=0;i<index;i++){

			for(int j=0;j<=i;j++){
				double totalDist=0;
				double totalVel=0;
				numberOfAlivePoints=0;
				for(int k=0; k<MAX_NUMBER_OF_POINTS_TO_TRACK;k++){
					if(isAlivePoint[i][k] && isAlivePoint[j][k]){
						double dist = sqrt( pow(featurePoints[i][k].x-featurePoints[j][k].x,2)+
								pow(featurePoints[i][k].y-featurePoints[j][k].y,2) );
						double vel ;//= (i==j ? 0 : dist/(i-j));
						if(i==j)
							vel=0;
						else
							vel = dist/(i-j);
						totalDist += dist;
						totalVel += vel;
						numberOfAlivePoints++;
					}
				}
				ssmPosFrame(i,j)=totalDist/numberOfAlivePoints;
				ssmPosFrame(j,i)=ssmPosFrame(i,j);
				ssmVelFrame(i,j)=totalVel;
				ssmVelFrame(j,i)=ssmVelFrame(i,j);
				std::cout << numberOfAlivePoints << " ";
			}
		}
		std::cout << std::endl << numberOfAlivePoints << " --\n";
		double max_dist = 0 , max_vel=0;
		double min_dist = UINT_MAX , min_vel = UINT_MAX;
		for(int i=0;i<index;i++)
			for(int j=0;j<index;j++){
				if(ssmPosFrame(i,j)>max_dist)
					max_dist=ssmPosFrame(i,j);
				else if(ssmPosFrame(i,j)<min_dist)
					min_dist=ssmPosFrame(i,j);
				if(ssmVelFrame(i,j)>max_vel)
					max_vel=ssmVelFrame(i,j);
				else if(ssmVelFrame(i,j)<min_vel)
					min_vel=ssmVelFrame(i,j);
				}
		std::cout << "MAX: " << max_dist << std::endl;
//		ssmPosFrame = (ssmPosFrame-min_dist)/(max_dist-min_dist)*255;
//		ssmVelFrame = (ssmVelFrame-min_vel)/(max_vel-min_vel)*255;
#ifdef KLT_DEBUG
		std::cout <<"\n\n";
#endif
		for(int i=0;i<index;i++){
			for(int j=0;j<index;j++){
				ssmPosFrame(i,j) = (ssmPosFrame(i,j)-min_dist)/(max_dist-min_dist)*255;
				ssmVelFrame(i,j) = (ssmVelFrame(i,j)-min_vel)/(max_vel-min_vel)*255;
#ifdef KLT_DEBUG
				std::cout << (int)ssmPosFrame(i,j) <<" ";
#endif
			}
#ifdef KLT_DEBUG
			std::cout <<"\n";
#endif
		}

//		_SSMPos(index,index);
//		_SSMVel(index,index);
//		resize(ssmFrame,ssmFrame,Size(500,500));
		ssmPosFrame.assignTo(_SSMPos,CV_8UC1);
		ssmVelFrame.assignTo(_SSMVel,CV_8UC1);
//		_SSMVel = ssmVelFrame;
#ifdef KLT_DEBUG
		std::cout << "Calculating SSMPos done.";
#endif
}

void KLTTracker::drawSSMPos(){
	drawSSM(_SSMPos);
}

void KLTTracker::drawSSMVel(){
	drawSSM(_SSMVel);
}

void KLTTracker::drawSSM(Mat_<uchar> grayFrame){
	resize(grayFrame,grayFrame,Size(500,500));
	Mat colorFrame;
	applyColorMap(grayFrame,colorFrame,COLORMAP_JET);

	imshow("Frame", colorFrame);
//		grayFrame.deallocate();
//	colorFrame.deallocate();
//	cvResizeWindow("Frame", 800, 800);
//	cvWaitKey();
}

Mat_<uchar> KLTTracker::getSSMPos(){
	return _SSMPos;
}

void KLTTracker::drawTrackedPoints(Mat &frame, int index,
		std::vector<cv::Point2f>  prevPoints,
		std::vector<cv::Point2f>  featurePoints) {
//		std::cout << prevPoints.size() << " ," << featurePoints.size() << "\n";

    for (uint i = 0; i < featurePoints.size(); i++) {
    	Scalar c(0,0,255);
    	if(!isAlivePoint[index][i]){
			c = Scalar(255,0,0);
        }
        line(frame,prevPoints[i], featurePoints[i], c);
		circle(frame, featurePoints[i], 3, c, -1);
    }
}
