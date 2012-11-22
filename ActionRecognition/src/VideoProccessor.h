/*
 * VideoProccessor.h
 *
 *  Created on: May 7, 2012
 *      Author: mohammad
 */

#include "BackgroundModel.h"
#include <opencv2/core/core.hpp>

#ifndef VIDEOPROCCESSOR_H_
#define VIDEOPROCCESSOR_H_

using namespace cv;

class VideoProccessor {
public:
	VideoProccessor();
	void removeBackground(Mat &frame, Mat_<Vec3f> &bMean, Mat_<Vec3f> &bVar);
	Mat_<Vec3f> nextFrame(int camera);
	void displayFrame(Mat &frame, int fps);
	void run(int camera);
//	void run(char* person, int camera)
	virtual ~VideoProccessor();
public:
	int file_index;
	BackgroundModel bModel;

};

#endif /* VIDEOPROCCESSOR_H_ */
