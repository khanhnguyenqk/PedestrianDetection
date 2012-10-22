#include "StdAfx.h"
#include "CaptureTrapezium.h"
#include "CvPoint_Wrapper.h"
#include "Eigen/Dense"

using namespace Eigen;

CaptureTrapezium::CaptureTrapezium(void)
{
	cornerLengthPercentage_ = 0.05;
	p1_ = p2_ = p3_ = p4_ = cvPoint2D32f(0.0, 0.0);
}


CaptureTrapezium::~CaptureTrapezium(void)
{
}

void CaptureTrapezium::finishCreating() {
	p1_ = cvPoint2D32f(rect_.x, rect_.y);
	p2_ = cvPoint2D32f(rect_.x+rect_.width, rect_.y);
	p3_ = cvPoint2D32f(rect_.x+rect_.width, rect_.y+rect_.height);
	p4_ = cvPoint2D32f(rect_.x, rect_.y+rect_.height);
}

int CaptureTrapezium::actionController(CvPoint mousePointer) {
	if (this->contains(mousePointer)) {
		//double sqrtP = sqrt(cornerPercentage_);
		//CvPoint tl = cvPoint(rect_.x,rect_.y);
		//CvPoint tr = cvPoint(rect_.x+rect_.width,rect_.y);
		//CvPoint sizeVector = cvPoint(rect_.width, rect_.height);
		//CvPoint sizeVectorInvert = cvPoint(-rect_.width, rect_.height);
		////CvPoint center = divVector(addVectors(sizeVector,tl), 2);

		//	CvPoint tl_inner = addVectors(tl, mulVector(sizeVector, sqrtP));
		//if ((mousePointer.x <= tl_inner.x) && (mousePointer.y <= tl_inner.y))
		//	return RESIZE_TL;

		//CvPoint tr_inner = addVectors(tr, mulVector(sizeVectorInvert, sqrtP));
		//if ((mousePointer.x >= tr_inner.x) && (mousePointer.y <= tr_inner.y))
		//	return RESIZE_TR;

		//CvPoint br_inner = addVectors(tl, mulVector(sizeVector, 1 - sqrtP));
		//if ((mousePointer.x >= br_inner.x) && (mousePointer.y >= br_inner.y))
		//	return RESIZE_BR;

		//CvPoint bl_inner = addVectors(tr, mulVector(sizeVectorInvert, 1 - sqrtP));
		//if ((mousePointer.x <= bl_inner.x) && (mousePointer.y >= bl_inner.y))
		//	return RESIZE_BL;
		Vector2d mouse;
		mouse << mousePointer.x, mousePointer.y;
		Vector2d p1, p2, p3, p4;
		p1 << p1_.x, p1_.y;	p2 << p2_.x, p2_.y;	p3 << p3_.x, p3_.y;	p4 << p4_.x, p4_.y;
		Vector2d diag1, diag2;
		diag1 = p3 - p1; diag2 = p4 - p2;
		double l1, l2;
		l1 = diag1.norm(); l2 = diag2.norm();
		double cornerZone1, cornerZone2;
		cornerZone1 = l1 * cornerLengthPercentage_; cornerZone2 = l2 * cornerLengthPercentage_;
		int ret = -1;
		if ((mouse - p1).norm() <= cornerZone1) {
			ret = P1;
		} else if ((mouse - p3).norm() <= cornerZone1) {
			ret = P3;
		} else if ((mouse - p2).norm() <= cornerZone2) {
			ret = P2;
		} else if ((mouse - p4).norm() <= cornerZone2) {
			ret = P4;
		}
		printf("%d\n", ret);
		if (ret != -1) return ret;
		return MOVE_RECT;
	}
	else
		return -1;
}

void CaptureTrapezium::moveCorner(int drawMethod, CvPoint vector) {
	switch (drawMethod) {
	case P1:
		p1_ = addVectors(p1_, vector);
		break;
	case P2:
		p2_ = addVectors(p2_, vector);
		break;
	case P3:
		p3_ = addVectors(p3_, vector);
		break;
	case P4:
		p4_ = addVectors(p4_, vector);
		break;
	default:
		throw "Exception here, change to something meaningful please!";
	}
	fixBoundaryRect();
}

void CaptureTrapezium::fixBoundaryRect() {
	int x1, y1, x2, y2;
	x1 = (int)minFour(p1_.x, p2_.x, p3_.x, p4_.x);
	y1 = (int)minFour(p1_.y, p2_.y, p3_.y, p4_.y);
	x2 = (int)maxFour(p1_.x, p2_.x, p3_.x, p4_.x);
	y2 = (int)maxFour(p1_.y, p2_.y, p3_.y, p4_.y);
	rect_.x = x1; rect_.y = y1;
	rect_.width = x2 - x1;
	rect_.height = y2 - y1;
}

void CaptureTrapezium::move(CvPoint vector) {
	rect_.x += vector.x; rect_.y += vector.y;
	p1_ = addVectors(p1_, vector);
	p2_ = addVectors(p2_, vector);
	p3_ = addVectors(p3_, vector);
	p4_ = addVectors(p4_, vector);
}

double CaptureTrapezium::maxFour(double a, double b, double c, double d) {
	return (a>b&&a>c&&a>d?a:b>c&&b>d?b:c>d?c:d);
}

double CaptureTrapezium::minFour(double a, double b, double c, double d) {
	return (a<b&&a<c&&a<d?a:b<c&&b<d?b:c<d?c:d);
}

CvPoint2D32f CaptureTrapezium::getP1() {
	return p1_;
}

CvPoint2D32f CaptureTrapezium::getP2() {
	return p2_;
}

CvPoint2D32f CaptureTrapezium::getP3() {
	return p3_;
}

CvPoint2D32f CaptureTrapezium::getP4() {
	return p4_;
}