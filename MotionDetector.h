// Main function: receive sequence of frames, return pieces of information about
//	moving objects.
// Define pieces of information:
// 
//

#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include "ExLBMixtureOfGaussians.h"

using namespace std;
using namespace cv;

class MotionDetector
{
protected:
	CvSize imgSize_;
	bool first_;

// Mixture of Gaussians
  ExLBMixtureOfGaussians *mog_;
//
//
	CvSeq* findContours(IplImage *singleChannelPic);
public:
	MotionDetector(CvSize imgSize);
	virtual ~MotionDetector(void);

	IplImage* processImage(IplImage* frame);
};