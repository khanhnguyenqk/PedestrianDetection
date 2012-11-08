// Vector calculation on CvPoint
#ifndef CVPOINT_WRAPPER_H_
#define CVPOINT_WRAPPER_H_

#include <opencv2/opencv.hpp>
#include <math.h>

using namespace std;

inline CvPoint addVectors(CvPoint a, CvPoint b) {
	return cvPoint(a.x + b.x, a.y + b.y);
}

inline CvPoint2D32f addVectors(CvPoint2D32f a, CvPoint2D32f b) {
	return cvPoint2D32f(a.x + b.x, a.y + b.y);
}

inline CvPoint2D32f addVectors(CvPoint2D32f a, CvPoint b) {
	CvPoint temp = addVectors(cvPoint((int)a.x, (int)a.y), b);
	return cvPoint2D32f(temp.x, temp.y);
}

inline CvPoint subVectors(CvPoint a, CvPoint b) {
		return cvPoint(a.x - b.x, a.y - b.y);
}

inline CvPoint2D32f subVectors(CvPoint2D32f a, CvPoint2D32f b) {
	return cvPoint2D32f(a.x - b.x, a.y - b.y);
}

inline CvPoint mulVector(CvPoint a, double v) {
	return cvPoint((int)(a.x*v),(int)(a.y*v));
}

inline CvPoint2D32f mulVector(CvPoint2D32f a, double v) {
  return cvPoint2D32f(a.x*v,a.y*v);
}

inline CvPoint divVector(CvPoint a, double v) {
	if (v == 0.0)
		exit (0);
	return cvPoint((int)(a.x/v),(int)(a.y/v));
}

inline CvPoint fromCvPoint2D32f(CvPoint2D32f p) {
	return cvPoint((int)p.x, (int)p.y);
}

inline double getMagnitude(CvPoint p) {
  return sqrt(pow((double)p.x, 2.0) + pow((double)p.y, 2.0));
}

inline CvPoint2D32f getDirection(CvPoint p) {
  if (!p.x && !p.y)
    return cvPoint2D32f(0.0, 0.0);
  CvPoint2D32f ret;
  double mag = getMagnitude(p);
  ret.x = ((double)p.x) / mag;
  ret.y = ((double)p.y) / mag;
  return ret;
}

#endif