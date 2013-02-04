/*
 * LogPolarDescriptor.h
 *
 *  Created on: Jan 26, 2013
 *      Author: mohammad
 */

#ifndef LOGPOLARDESCRIPTOR_H_
#define LOGPOLARDESCRIPTOR_H_

#include <cv.h>

namespace IPPR {

class LogPolarDescriptor {
public:
	LogPolarDescriptor();
	virtual ~LogPolarDescriptor();
	void buildDescriptor(cv::Mat image, int radius);
	void saveDescriptor(std::string, std::string);
	void showDescriptor();

//private:
	std::vector<float> buildLocalLogPolarDescriptor(cv::Mat image, cv::Point2d center, int radius);
	std::vector<float> hist(cv::Mat image, cv::Mat_<uchar> mask, int nbins);

	std::vector<std::vector<float> > descriptor;
};

} /* namespace IPPR */
#endif /* LOGPOLARDESCRIPTOR_H_ */
