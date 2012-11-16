#pragma once
#include <opencv2/opencv.hpp>
#include "CvPoint_Wrapper.h"

using namespace std;

enum {NEW_LINE, MOVE_LINE, LINE_P1, LINE_P2};

class DrawToolLine
{
protected:
  CvScalar color_;
  CvScalar *colorOrg_;
  int thickness_;
  int lineType_;
  int mouseDistanceAccepted_;
public:
  CvPoint p1_;
  CvPoint p2_;
public:
  DrawToolLine(void);
  virtual ~DrawToolLine(void);
  void setLine(CvPoint p1, CvPoint p2);
  void setColor(CvScalar color);
  void darkenColor();
  void returnOriginalColor();
  void move(CvPoint vector);
  void modify(int drawMethod, CvPoint vector);
  int actionController(CvPoint mousePointer);
  void drawSelfOnImage(IplImage *img);
};

