/*
 * quiver.h
 *
 *  Created on: Jan 2, 2013
 *      Author: mohammad
 */

#ifndef QUIVER_H_
#define QUIVER_H_

#include "OpticalFlowProcessor.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include <iostream>
#include <cstdlib>
#include <stdio.h>

using namespace std;
using namespace cv;

void cvQuiver(const cv::Mat& flow,
        cv::Mat& frame,
        int step,
        const cv::Scalar& color) {
	vector<Mat_<float> > planes;
	split( flow, planes);
	Mat_<float> velx = planes[0];
	Mat_<float> vely = planes[1];
	double l_max = -10;
	for (int y = step; y < frame.rows-step; y += step) // First iteration, to compute the maximum l (longest flow)
			{
		for (int x = step; x < frame.cols-step; x += step) {
			float dx = velx[y][x];  // Gets X component of the flow
			float dy = vely[y][x];  // Gets Y component of the flow

			CvPoint p = cvPoint(x, y);

			float l = sqrt(dx * dx + dy * dy); // This function sets a basic threshold for drawing on the image

			if (l > l_max)
				l_max = l;
		}
	}
//
	for (int y = step; y < frame.rows-step; y += step) {
		for (int x = step; x < frame.cols-step; x += step) {
			float dx = velx[y][x];  // Gets X component of the flow
			float dy = vely[y][x];  // Gets Y component of the flow

			CvPoint p = cvPoint(x, y);

			float l = sqrt(dx * dx + dy * dy); // This function sets a basic threshold for drawing on the image
			if (l > 0) {
				float spinSize = 2.0 * l / l_max; // Factor to normalize the size of the spin depending on the length of the arrow
				CvPoint p2 = cvPoint(p.x + (int) (dx), p.y + (int) (dy));
				line(frame, p, p2, CV_RGB(0,255,0), 1, CV_AA);

				double angle; // Draws the spin of the arrow
				angle = atan2((double) p.y - p2.y, (double) p.x - p2.x);

				p.x = (int) (p2.x + spinSize * cos(angle + 3.1416 / 4));
				p.y = (int) (p2.y + spinSize * sin(angle + 3.1416 / 4));
				line(frame, p, p2, CV_RGB(0,255,0), 1, CV_AA, 0);

				p.x = (int) (p2.x + spinSize * cos(angle - 3.1416 / 4));
				p.y = (int) (p2.y + spinSize * sin(angle - 3.1416 / 4));
				line(frame, p, p2, CV_RGB(0,255,0), 1, CV_AA, 0);
			}
		}
	}
}

void drawOptFlowMap(const cv::Mat& flow,
                    cv::Mat& cflowmap,
                    int step,
                    const cv::Scalar& color
                   )
{
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const cv::Point2f& fxy = flow.at<cv::Point2f>(y, x);
            cv::line(cflowmap,
                             cv::Point(x,y),
                             cv::Point(cvRound(x+fxy.x),cvRound(y+fxy.y)),
                 color);
            cv::circle(cflowmap, cv::Point(x,y), 1, color, -1);
        }
}


#endif /* QUIVER_H_ */
