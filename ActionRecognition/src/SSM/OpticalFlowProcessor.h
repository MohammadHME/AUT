/*
 * OpticalFlowProcessor.h
 *
 *  Created on: Dec 31, 2012
 *      Author: mohammad
 */

#ifndef OPTICALFLOWPROCESSOR_H_
#define OPTICALFLOWPROCESSOR_H_

#include <opencv2/core/core.hpp>
#include "../stdafx.h"

namespace IPPR {

class OpticalFlowProcessor {
public:
	OpticalFlowProcessor();
	virtual ~OpticalFlowProcessor();

	void begin();
	void drawSSMOF();
	void calculateSSMOF();
	cv::Mat_<uchar> getSSMOF();

	void process(cv::Mat &frame, cv::Mat_<uchar> mask);

private:
	void drawSSM(cv::Mat_<uchar>);
	cv::Mat_<uchar> _SSMOF;

    std::vector<cv::Mat> _flowVector;
    cv::Mat _prevGrayFrame;
    int _frameCounter;
};

} /* namespace IPPR */
#endif /* OPTICALFLOWPROCESSOR_H_ */
