/*
 * SilhouetteProcessor.h
 *
 *  Created on: Jan 7, 2013
 *      Author: mohammad
 */

#ifndef SILHOUETTEPROCESSOR_H_
#define SILHOUETTEPROCESSOR_H_

#include <opencv2/core/core.hpp>
#include "../stdafx.h"

namespace IPPR {

class SilhouetteProcessor {
public:
	SilhouetteProcessor();
	virtual ~SilhouetteProcessor();
	void begin();
	void drawSSM();
	void calculateSSM();
	cv::Mat_<uchar> getSSM(){return _SSM;};
	void process(cv::Mat &frame, cv::Mat_<uchar> mask);
private:
	void drawSSM(cv::Mat_<uchar>);
	std::vector<cv::Mat_<uchar> > silhouettes;
	cv::Mat_<uchar> _SSM;
};

} /* namespace IPPR */
#endif /* SILHOUETTEPROCESSOR_H_ */
