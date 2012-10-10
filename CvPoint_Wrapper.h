#ifndef CVPOINT_WRAPPER_H_
#define CVPOINT_WRAPPER_H_

#include <opencv2/opencv.hpp>

inline CvPoint addVectors(CvPoint a, CvPoint b) {
	return cvPoint(a.x + b.x, a.y + b.y);
}

inline CvPoint subVectors(CvPoint a, CvPoint b) {
		return cvPoint(a.x - b.x, a.y - b.y);
}

inline CvPoint mulVector(CvPoint a, double v) {
	return cvPoint((int)(a.x*v),(int)(a.y*v));
}

inline CvPoint divVector(CvPoint a, double v) {
	if (v == 0.0)
		exit (0);
	return cvPoint((int)(a.x/v),(int)(a.y/v));
}

#endif