#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include "CvPoint_Wrapper.h"

using namespace std;
using namespace cv;

class ForegroundObject
{
protected:
  string label_;
  vector<CvPoint> positionHistory_;
  vector<CvPoint> predictedPositionHistory_;
  vector<CvPoint> correctedPositionHistory_;
  KalmanFilter *kalman_;
public:
  static double speedUpdateRate_;
  ForegroundObject(string label, CvPoint position);
  virtual ~ForegroundObject(void);
  CvPoint correctPosition(CvPoint newPosition);
  CvPoint predictNextPosition();
};