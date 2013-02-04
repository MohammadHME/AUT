/*
 * Test.h
 *
 *  Created on: Jan 7, 2013
 *      Author: mohammad
 */

#ifndef TEST_H_
#define TEST_H_

#include <cv.h>

namespace IPPR {

class Test {
public:
	Test();
	virtual ~Test();
	void run();
	void logPolarMask(cv::Point2d center, int radius);
	std::vector<float> hist(cv::Mat frame, cv::Mat_<uchar> mask, int nbins);
};

} /* namespace IPPR */
#endif /* TEST_H_ */
