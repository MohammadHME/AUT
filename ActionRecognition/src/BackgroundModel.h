/*
 * BackgroundModel.h
 *
 *  Created on: May 4, 2012
 *      Author: mohammad
 */
#include <opencv2/core/core.hpp>

#ifndef BACKGROUNDMODEL_H_
#define BACKGROUNDMODEL_H_

class BackgroundModel {
public:
	cv::Mat_<cv::Vec3f> bMean[5], bMean_hsv[5];
	cv::Mat_<cv::Vec3f> bVar[5], bVar_hsv[5];
	cv::Mat_<cv::Vec3f> bStd[5], bStd_hsv[5];
public:
	BackgroundModel();
	virtual ~BackgroundModel();
	void buildAllBackgroundsModel();
	void saveBackgroundsModel();
	void loadBackgroundsModel();
	void buildBackgroundModel(char* dir, int camera, int framecounts);
	cv::Mat_<cv::Vec3f> getBackgroundMean(int camera){return bMean[camera];};
	cv::Mat_<cv::Vec3f> getBackgroundVar(int camera){return bVar[camera];};
	cv::Mat_<cv::Vec3f> getBackgroundStd(int camera){return bStd[camera];};
};

#endif /* BACKGROUNDMODEL_H_ */
