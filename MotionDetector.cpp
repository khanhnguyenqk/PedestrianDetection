#include "StdAfx.h"
#include "MotionDetector.h"

MotionDetector::MotionDetector(CvSize imgSize)
{
  // Mixture of Gaussians
  mog_ = new ExLBMixtureOfGaussians;
}


MotionDetector::~MotionDetector(void)
{
}

IplImage* MotionDetector::processImage(IplImage* frame) {
	IplImage *ret = cvCloneImage(frame);

	/*try {
		cvReleaseImage(&diff);
	} catch (...) {}*/
	return ret;
}