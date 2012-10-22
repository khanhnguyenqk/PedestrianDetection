// Vector calculation on CvPoint
#ifndef CVPOINT_WRAPPER_H_
#define CVPOINT_WRAPPER_H_

#include <opencv2/opencv.hpp>

inline CvPoint addVectors(CvPoint a, CvPoint b) {
	return cvPoint(a.x + b.x, a.y + b.y);
}


inline CvPoint2D32f addVectors(CvPoint2D32f a, CvPoint b) {
	CvPoint temp = addVectors(cvPoint((int)a.x, (int)a.y), b);
	return cvPoint2D32f(temp.x, temp.y);
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

inline CvPoint fromCvPoint2D32f(CvPoint2D32f p) {
	return cvPoint((int)p.x, (int)p.y);
}

#endif