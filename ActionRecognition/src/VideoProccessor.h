/*
 * VideoProccessor.h
 *
 *  Created on: May 7, 2012
 *      Author: mohammad
 */

#include "BackgroundModel.h"
#include <opencv2/core/core.hpp>
#include <string>

#ifndef VIDEOPROCCESSOR_H_
#define VIDEOPROCCESSOR_H_

using namespace cv;

class VideoProccessor {
public:
	VideoProccessor();
	Mat_<uchar> removeBackground(Mat &frame, Mat_<Vec3f> &bMean
			, Mat_<Vec3f> &bVar,  Mat_<Vec3f> &bStd);
	void removeShadow(Mat &frame,Mat_<uchar> &forground, Mat_<Vec3f> &bMean
			, Mat_<Vec3f> &bVar,  Mat_<Vec3f> &bStd);
	Mat_<uchar> findBiggestBlob(Mat_<uchar>  &forground);
//	void removeBackgroundRGB(Mat &frame, Mat_<Vec3f> &bMean
//				, Mat_<Vec3f> &bVar,  Mat_<Vec3f> &bStd);
	Mat nextFrame(string actor, string camera,int frameIndex, Mat_<uchar> &mask);
	void displayFrame(Mat &frame, std::string actor,
			std::string action, std::string camera, int fps);
	void displayFrame(Mat &frame, int fps);
	void saveMatrix(Mat_<uchar> matrix,string type, string actor,string action,
			string camera,int startFrame,int endFrame);
	void run(string actor,string action,string camera,
			int startFrame,int endFrame);
	virtual ~VideoProccessor();
public:
	int file_index;
	BackgroundModel bModel;

};

#endif /* VIDEOPROCCESSOR_H_ */
