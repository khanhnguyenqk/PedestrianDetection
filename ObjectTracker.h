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

class ObjectTracker
{
protected:
  int openIteration_, closeIteration_;
	CvSize imgSize_;
	bool first_;
  IplImage *mask3C_, *mask_, *temp_;

  ExLBMixtureOfGaussians *mog_;

	CvSeq* findContours(IplImage *singleChannelPic);
  void findConnectedComponents(
    IplImage* mask,
    int poly1_hull2 = 0,
    float perimScale = 0.25,
    int* num = NULL,
    CvRect* bbs = NULL,
    CvPoint* centers = NULL
    );
public:
	ObjectTracker(CvSize imgSize);
	virtual ~ObjectTracker(void);

  void processImage(IplImage* frame, IplImage** output);
};