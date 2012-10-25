#include "StdAfx.h"
#include "AOITrapezium.h"
#include "CvPoint_Wrapper.h"
#include "Eigen/Dense"
#include "LinearAlgebra.h"

using namespace Eigen;

AOITrapezium::AOITrapezium(void)
{
	cornerLengthPercentage_ = 0.05;
	pts_[0] = pts_[1] = pts_[2] = pts_[3] = cvPoint2D32f(0.0, 0.0);
}


AOITrapezium::~AOITrapezium(void)
{
}

void AOITrapezium::reassignedCorners() {
	pts_[0] = cvPoint2D32f(rect_.x, rect_.y);
	pts_[1] = cvPoint2D32f(rect_.x+rect_.width, rect_.y);
	pts_[2] = cvPoint2D32f(rect_.x+rect_.width, rect_.y+rect_.height);
	pts_[3] = cvPoint2D32f(rect_.x, rect_.y+rect_.height);
}

int AOITrapezium::actionController(CvPoint mousePointer, bool callParent) {
	if (this->contains(mousePointer)) {
		RowVector2d mouse;
		mouse << mousePointer.x, mousePointer.y;
		RowVector2d pts[4];
		pts[0] << pts_[0].x, pts_[0].y;	pts[1] << pts_[1].x, pts_[1].y;	
		pts[2] << pts_[2].x, pts_[2].y;	pts[3] << pts_[3].x, pts_[3].y;
		RowVector2d diag1, diag2;
		diag1 = pts[2] - pts[0]; diag2 = pts[3] - pts[1];
		double l1, l2;
		l1 = diag1.norm(); l2 = diag2.norm();
		double cornerZone1, cornerZone2;
		cornerZone1 = l1 * cornerLengthPercentage_; cornerZone2 = l2 * cornerLengthPercentage_;
		int ret = -1;
		if ((mouse - pts[0]).norm() <= cornerZone1) {
			ret = P0;
		} else if ((mouse - pts[2]).norm() <= cornerZone1) {
			ret = P2;
		} else if ((mouse - pts[1]).norm() <= cornerZone2) {
			ret = P1;
		} else if ((mouse - pts[3]).norm() <= cornerZone2) {
			ret = P3;
		}
		if (ret != -1) return ret;
		return MOVE_RECT;
	}
	else
		return -1;
}

void AOITrapezium::moveCorner(int drawMethod, CvPoint vector) {
	CvPoint2D32f dest;
	CvPoint2D32f newPoints[4];
	switch (drawMethod) {
	case RESIZE_TL: case RESIZE_TR: case RESIZE_BR: case RESIZE_BL:
		pts_[0] = pts_[1] = pts_[2] = pts_[3] = cvPoint2D32f(0.0, 0.0);
		AOIRect::moveCorner(drawMethod, vector);
		reassignedCorners();
		break;
	case P0:
		dest = addVectors(pts_[0], vector);
		newPoints[0] = dest; 
		newPoints[1] = pts_[1]; newPoints[2] = pts_[2]; newPoints[3] = pts_[3];
		if (isConvex(newPoints)) {
			pts_[0] = addVectors(pts_[0], vector);
		}
		fixBoundaryRect();
		break;
	case P1:
		dest = addVectors(pts_[1], vector);
		newPoints[1] = dest; 
		newPoints[0] = pts_[0]; newPoints[2] = pts_[2]; newPoints[3] = pts_[3];
		if (isConvex(newPoints)) {
			pts_[1] = addVectors(pts_[1], vector);
		}
		fixBoundaryRect();
		break;
	case P2:
		dest = addVectors(pts_[2], vector);
		newPoints[2] = dest; 
		newPoints[1] = pts_[1]; newPoints[0] = pts_[0]; newPoints[3] = pts_[3];
		if (isConvex(newPoints)) {
			pts_[2] = addVectors(pts_[2], vector);
		}
		fixBoundaryRect();
		break;
	case P3:
		dest = addVectors(pts_[3], vector);
		newPoints[3] = dest; 
		newPoints[1] = pts_[1]; newPoints[2] = pts_[2]; newPoints[0] = pts_[0];
		if (isConvex(newPoints)) {
			pts_[3] = addVectors(pts_[3], vector);
		}
		fixBoundaryRect();
		break;
	default:
		throw "Exception here, change to something meaningful please!";
	}
}

void AOITrapezium::fixBoundaryRect() {
	int x1, y1, x2, y2;
	x1 = (int)minFour(pts_[0].x, pts_[1].x, pts_[2].x, pts_[3].x);
	y1 = (int)minFour(pts_[0].y, pts_[1].y, pts_[2].y, pts_[3].y);
	x2 = (int)maxFour(pts_[0].x, pts_[1].x, pts_[2].x, pts_[3].x);
	y2 = (int)maxFour(pts_[0].y, pts_[1].y, pts_[2].y, pts_[3].y);
	rect_.x = x1; rect_.y = y1;
	rect_.width = x2 - x1;
	rect_.height = y2 - y1;
}

void AOITrapezium::move(CvPoint vector) {
	rect_.x += vector.x; rect_.y += vector.y;
	pts_[0] = addVectors(pts_[0], vector);
	pts_[1] = addVectors(pts_[1], vector);
	pts_[2] = addVectors(pts_[2], vector);
	pts_[3] = addVectors(pts_[3], vector);
}

double AOITrapezium::maxFour(double a, double b, double c, double d) {
	return (a>b&&a>c&&a>d?a:b>c&&b>d?b:c>d?c:d);
}

double AOITrapezium::minFour(double a, double b, double c, double d) {
	return (a<b&&a<c&&a<d?a:b<c&&b<d?b:c<d?c:d);
}

CvPoint2D32f AOITrapezium::getPoint(int it) {
	return pts_[it];
}

bool AOITrapezium::isConvex(CvPoint2D32f pts[4]) {
	return (!areOnSameSide(pts[0], pts[2], pts[1], pts[3])) && 
		(!areOnSameSide(pts[1], pts[3], pts[0], pts[2]));
}

void AOITrapezium::drawSelfOnImage(IplImage* img) {
	int lineType = 20;
	AOIRect::drawSelfOnImage(img);
	cvLine(img, fromCvPoint2D32f(pts_[0]), fromCvPoint2D32f(pts_[1]), color_, thickness_, lineType);
	cvLine(img, fromCvPoint2D32f(pts_[1]), fromCvPoint2D32f(pts_[2]), color_, thickness_, lineType);
	cvLine(img, fromCvPoint2D32f(pts_[2]), fromCvPoint2D32f(pts_[3]), color_, thickness_, lineType);
	cvLine(img, fromCvPoint2D32f(pts_[3]), fromCvPoint2D32f(pts_[0]), color_, thickness_, lineType);
}