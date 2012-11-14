// Main function: receive sequence of frames, return pieces of information about
//	moving objects.
// Define pieces of information:
// 
//

#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include <vector>
#include "ExLBMixtureOfGaussians.h"
#include "ForegroundObject.h"

using namespace std;
using namespace cv;

class ObjectTracker
{
protected:
  // Foreground detection
  int openIteration_, closeIteration_;
	CvSize imgSize_;
	bool first_;
  IplImage *mask3C_, *mask_, *temp_;
  ExLBMixtureOfGaussians *mog_;
  CvSeq* findContours(IplImage *singleChannelPic);
  double perimScaleThrhold_;

  // Objects tracker
  double matchThreshold_;
  long int numObjs_;
  vector<ForegroundObject*> currObjs_;
  double **objsM_;
  CvPoint *centersStat_;
  CvRect *rectsStat_;
  int numStat_, count_;
  void findConnectedComponents(
    IplImage* mask,
    int poly1_hull2 = 0,
    double perimScale = 0.25,
    int* num = NULL,
    CvRect* bbs = NULL,
    CvPoint* centers = NULL
    );
  void matchObjects(CvPoint *newCenters, int size);
  void createNewObject(CvPoint center);
  void removeObjects(vector<int> *iterators = NULL);
public:
	ObjectTracker(CvSize imgSize);
	virtual ~ObjectTracker(void);

  void processImage(IplImage* frame, IplImage** output);
};