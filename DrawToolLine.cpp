#include "StdAfx.h"
#include "DrawToolLine.h"


DrawToolLine::DrawToolLine(void)
{
  colorOrg_ = NULL;
  color_ = cvScalar(255,0,0);
  thickness_ = 1;
  mouseDistanceAccepted_ = 5;
  lineType_ = 20;
}

DrawToolLine::~DrawToolLine(void)
{
  if (colorOrg_)
    delete colorOrg_;
}

void DrawToolLine::setLine(CvPoint p1, CvPoint p2) {
  p1_ = p1;
  p2_ = p2;
}

void DrawToolLine::setColor(CvScalar color) {
  color_ = color;
}

void DrawToolLine::darkenColor() {
  if (colorOrg_ == NULL) {
    colorOrg_ = new CvScalar;
    *colorOrg_ = color_;
    double* val = color_.val;
    color_ = cvScalar(val[0]/2, val[1]/2, val[2]/2);
  }
}

void DrawToolLine::returnOriginalColor() {
  if (colorOrg_ != NULL) {
    color_ = *colorOrg_;
    delete colorOrg_;
    colorOrg_ = NULL;
  }
}

void DrawToolLine::move(CvPoint vector) {
  p1_ = addVectors(p1_, vector);
  p2_ = addVectors(p2_, vector);
}

void DrawToolLine::modify(int drawMethod, CvPoint vector) {
  switch (drawMethod) {
  case LINE_P1:
    p1_ = addVectors(p1_, vector);
    break;
  case LINE_P2:
    p2_ = addVectors(p2_, vector);
    break;
  case MOVE_LINE:
    move(vector);
    break;
  default:
    throw "Unknown action on object";
  }
}

int DrawToolLine::actionController(CvPoint mousePointer) {
  if (getMagnitude(subVectors(mousePointer, p1_)) <= mouseDistanceAccepted_) {
    return LINE_P1;
  }
  if (getMagnitude(subVectors(mousePointer, p2_)) <= mouseDistanceAccepted_) {
    return LINE_P2;
  }
  CvPoint mid = cvPoint((p1_.x + p2_.x) / 2, (p1_.y + p2_.y) / 2);
  if (getMagnitude(subVectors(mousePointer, mid)) <= mouseDistanceAccepted_) {
    return MOVE_LINE;
  }
  return -1;
}

void DrawToolLine::drawSelfOnImage(IplImage *img) {
  cvLine(img, p1_, p2_, color_, thickness_, lineType_);
  CvPoint center = cvPoint((p1_.x+p2_.x)/2, (p1_.y+p2_.y)/2);
  cvCircle(img, center, mouseDistanceAccepted_, color_, thickness_, lineType_);
}