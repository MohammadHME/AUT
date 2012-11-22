/*
 * KLTTracker.h
 *
 *  Created on: May 11, 2012
 *      Author: mohammad
 */

#ifndef KLTTRACKER_H_
#define KLTTRACKER_H_

#include <opencv2/core/core.hpp>
#include "stdafx.h"

//class Features{
//public:
//	std::vector<std::vector<cv::Point2f> > points;
//};

class KLTTracker {
public:
	KLTTracker();
	virtual ~KLTTracker();
	void begin();
	void drawSSMPos();
	void drawSSMVel();
	void calculateSSMPos();
	cv::Mat_<uchar> getSSMPos();
	cv::Mat_<uchar> getSSMVel();
	void process(cv::Mat &frame);
	void drawTrackedPoints(cv::Mat &frame, std::vector<cv::Point2f>  prevPoints,std::vector<cv::Point2f>  featurePoints);
private:
	void drawSSM(cv::Mat_<uchar>);
	std::vector<cv::Point2f> points[2]; // initial position of tracked points
	std::vector<cv::Point2f> initial;

	std::vector<std::vector<cv::Point2f> > featurePoints;
	int index;
//	std::vector< std::vector<bool> > goodPoints;
	bool isAlivePoint[MAX_ACTION_LENGTH][MAX_NUMBER_OF_POINTS_TO_TRACK];


	cv::Mat_<uchar> _SSMPos, _SSMVel;


//    Features features;

    int number_of_points; // maximum number of points to detect
    double qlevel; // quality level for feature detection
    double min_dist_btw_Points; // min distance between two points
    std::vector<uchar> status; // status of tracked features
    std::vector<float> err; // error in tracking
    cv::Mat prevGrayFrame;
};

#endif /* KLTTRACKER_H_ */
