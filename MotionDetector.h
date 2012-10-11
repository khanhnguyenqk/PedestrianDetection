// Main function: receive sequence of frames, return pieces of information about
//	moving objects.
// Define pieces of information:
// 
//
// 
//
// 

#pragma once

#include <opencv2/opencv.hpp>

using namespace std;

class MotionDetector
{
protected:
	CvSize imgSize_;
	IplImage* greyImage_;
	IplImage* colourImage_;
	IplImage* movingAverage_;
	IplImage* difference_;
	IplImage* temp_;
	IplImage* motionHistory_;

	// Parameters
	double alpha_;

	int prevX_;
	int numObj_;
	bool first_;
public:
	MotionDetector(CvSize imgSize);
	virtual ~MotionDetector(void);

	IplImage* processPicture(IplImage* frame);
};

