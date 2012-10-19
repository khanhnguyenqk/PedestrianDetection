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

enum {SIMPLE, BKGD_AVE};

class MotionDetector
{
protected:
	int methodCode_;
	CvSize imgSize_;
	bool first_;

// Simple method
//
// 
protected:
	IplImage* sGreyImage_;
	IplImage* sMovingAverage_;
	IplImage* sDifference_;
	IplImage* sTemp_;

	// Parameters
	double alpha_;
	unsigned thresvalue_;
	unsigned dilateIterations_;
	unsigned erodeIterations_;

	int prevX_;
	int numObj_;

	IplImage* sBackGroudDiff(IplImage *frame);

// Background Average
//
// 
protected:
	int bkgdAccCircle_, circleCount_;
	double count_;
	double upperScale_, lowerScale_;
	//Float, 3-channel images
	IplImage *total_, *totalDiff_, *mean_, *deviation_, *prev_, *hi_, *low_;
	IplImage *temp1_, *temp2_;

	//Float, 1-channel images
	IplImage *gray1_, *gray2_, *gray3_;
	IplImage *low1_, *low2_, *low3_;
	IplImage *hi1_, *hi2_, *hi3_;

	//Unsigned, 1-channel images
	IplImage *mask1_, *mask2_;

	void ba_accumulateBackground(IplImage *frame);
	void ba_createModelsFromStats();
	void ba_setThreshold();
	void ba_backGroundDiff(IplImage *frame);
	IplImage* ba_wrapper(IplImage *frame);
//
//
	CvSeq* findContours(IplImage *singleChannelPic);
public:
	MotionDetector(CvSize imgSize);
	virtual ~MotionDetector(void);

	IplImage* processImage(IplImage* frame);
};