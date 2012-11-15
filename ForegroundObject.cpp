#include "StdAfx.h"
#include "ForegroundObject.h"

double ForegroundObject::speedUpdateRate_ = 0.5;

ForegroundObject::ForegroundObject(string label, CvPoint position) {
  alignIteration_ = 12;
  label_ = label;
  positionHistory_.clear();
  predictedPositionHistory_.clear();
  correctedPositionHistory_.clear();

  positionHistory_.push_back(position);

  kalman_ = new KalmanFilter(4, 2, 0);
  
  kalman_->statePre.at<float>(0) = position.x;
  kalman_->statePre.at<float>(1) = position.y;
  kalman_->statePre.at<float>(2) = 0;
  kalman_->statePre.at<float>(3) = 0;
  kalman_->transitionMatrix = *(Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);

  setIdentity(kalman_->measurementMatrix);
  setIdentity(kalman_->processNoiseCov, Scalar::all(1e-4));
  setIdentity(kalman_->measurementNoiseCov, Scalar::all(1e-1));
  setIdentity(kalman_->errorCovPost, Scalar::all(.1));

  // Align kalman predictions with the initial position
  for (int i=0; i<alignIteration_-1; i++) {
    Mat_<float> measurement(2,1);
    measurement(0) = (float)position.x;
    measurement(1) = (float)position.y;
    kalman_->predict();
    kalman_->correct(measurement);
  }
  this->predictNextPosition();
  this->correctPosition(position);
}

ForegroundObject::~ForegroundObject(void)
{
}

CvPoint ForegroundObject::predictNextPosition() {
  Mat prediction = kalman_->predict();
  CvPoint ret = cvPoint((int)prediction.at<float>(0), (int)prediction.at<float>(1));
  predictedPositionHistory_.push_back(ret);
  return ret;
}

CvPoint ForegroundObject::correctPosition(CvPoint newPosition) {
  positionHistory_.push_back(newPosition);
  Mat_<float> measurement(2,1);
  measurement(0) = (float)newPosition.x;
  measurement(1) = (float)newPosition.y;

  Mat estimated = kalman_->correct(measurement);
  CvPoint ret = cvPoint((int)estimated.at<float>(0), (int)estimated.at<float>(1));
  correctedPositionHistory_.push_back(ret);
  return ret;
}

bool ForegroundObject::isEqual(ForegroundObject another) {
  if (another.label_ != label_)
    return false;
  /*if (another.positionHistory_.size() != positionHistory_.size())
    return false;
  if (another.predictedPositionHistory_.size() != predictedPositionHistory_.size())
    return false;
  if (another.correctedPositionHistory_.size() != correctedPositionHistory_.size())
    return false;*/
  return true;
}