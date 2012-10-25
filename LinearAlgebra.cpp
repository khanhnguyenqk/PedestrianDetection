#include "stdafx.h"
#include "LinearAlgebra.h"
#include <Eigen/Dense>
#include <iostream>
#include <stdio.h>

using namespace Eigen;
using namespace std;

LineSegment2D::LineSegment2D() {
	line2D_.resize(3,1);
}

bool isNumber(double x) {
	return (x == x);
}

bool isFiniteNumber(double x) 
{
	return (x <= DBL_MAX && x >= -DBL_MAX);
}

bool isLine2D(MatrixXd m) {
	cout << "M: \"" << m << "\"\n";
	return (((m.rows() == 3) && (m.cols() == 1)) /*&& ((m(0,0) != 0) || (m(0,1) != 0))*/);
}

MatrixXd findLineFormular2D(CvPoint2D32f a, CvPoint2D32f b) {
	if (a.x == b.x && a.y == b.y) {
		throw UNSOLVABLE;
	}
	MatrixXd mRet(3,1);
	mRet << a.y - b.y,
		b.x - a.x,
		(a.x - b.x)*a.y + a.x*(b.y - a.y);
	return mRet;
}

LineSegment2D findLineSegmentFormular2D(CvPoint2D32f a, CvPoint2D32f b) {
	try {
		LineSegment2D ret;
		ret.line2D_ = findLineFormular2D(a, b);
		ret.a_ = a;
		ret.b_ = b;
		return ret;
	} catch (int e) {
		throw e;
	}
}

double findX(MatrixXd mA, double y) {
	if (isLine2D(mA)) {
		if (mA(0,0) == 0.0) {
			throw INFINTE_SOLUTION;
		}
		else
			return (-mA(2,0) -(mA(1,0) * y))/mA(0,0);
	} else {
		// Does not implement >2d yet
		throw UNSOLVABLE;
	}
}

double findY(MatrixXd mA, double x) {
	if (isLine2D(mA)) {
		if (mA(1,0) == 0.0) {
			throw INFINTE_SOLUTION;
		}
		else
			return (-mA(2,0) -(mA(0,0) * x))/mA(1,0);
	} else {
		// Does not implement >2d yet
		throw UNSOLVABLE;
	}
}

double findX(LineSegment2D mA, double y) {
	if ((y >= mA.a_.y && y <= mA.b_.y) ||
		(y >= mA.b_.y && y <= mA.a_.y)) {
			return findX(mA.line2D_, y);
	}
	throw UNSOLVABLE;
}

double findY(LineSegment2D mA, double x) {
	if ((x >= mA.a_.x && x <= mA.b_.x) ||
		(x >= mA.b_.x && x <= mA.a_.x)) {
			return findX(mA.line2D_, x);
	}
	throw UNSOLVABLE;
}

bool areOnSameSide(CvPoint2D32f first, CvPoint2D32f second,
					CvPoint2D32f linep1, CvPoint2D32f linep2) {
	RowVector3d a, b, c;
	a << first.x - linep1.x, first.y - linep1.y, 0;
	b << linep2.x - linep1.x, linep2.y - linep1.y, 0;
	c << second.x - linep1.x, second.y - linep1.y, 0;
	double x = (a.cross(b)).dot(c.cross(b));
	return x>0?true:false;
}