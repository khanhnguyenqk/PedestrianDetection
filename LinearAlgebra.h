#pragma once
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

using namespace Eigen;

enum {
	UNSOLVABLE,
	INFINTE_SOLUTION
};

struct LineSegment2D {
	MatrixXd line2D_;
	CvPoint2D32f a_;
	CvPoint2D32f b_;
	LineSegment2D();
};

bool isLine2D(MatrixXd m);
bool isNumber(double x);
bool isFiniteNumber(double x);
MatrixXd findLineFormular2D(CvPoint2D32f a, CvPoint2D32f b);
LineSegment2D findLineSegmentFormular2D(CvPoint2D32f a, CvPoint2D32f b);
double findX(MatrixXd mA, double y);
double findY(MatrixXd mA, double x);
double findX(LineSegment2D mA, double y);
double findY(LineSegment2D mA, double x);
bool areOnSameSide(CvPoint2D32f first, CvPoint2D32f second,
					CvPoint2D32f linep1, CvPoint2D32f linep2);
bool isOnOrBetween(CvPoint2D32f p, CvPoint2D32f line11, CvPoint2D32f line12,
          CvPoint2D32f line21, CvPoint2D32f line22);
bool isOnOrInConvex(CvPoint2D32f p, CvPoint2D32f p1, CvPoint2D32f p2,
  CvPoint2D32f p3, CvPoint2D32f p4);