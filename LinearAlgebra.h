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