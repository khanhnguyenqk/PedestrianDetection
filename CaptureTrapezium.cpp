#include "StdAfx.h"
#include "CaptureTrapezium.h"
#include "CvPoint_Wrapper.h"
#include "Eigen/Dense"

using namespace Eigen;

CaptureTrapezium::CaptureTrapezium(void)
{
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
		///*CvPoint center = divVector(addVectors(sizeVector,tl), 2);*/

		//CvPoint tl_inner = addVectors(tl, mulVector(sizeVector, sqrtP));
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
		Matrix2d p1, p2, p3, p4;
		p1 << p1_.x, p1_.y;		p2 << p2_.x, p2_.y;		p3 << p3_.x, p3_.y;		p4 << p4_.x, p4_.y;
		Matrix2d diag1, diag2;
		diag1 = p3 - p1;
		diag2 = p4 - p2;
		double l1, l2;
		l1 = diag1.norm(); l2 = diag2.norm();

		return MOVE_RECT;
	}
	else
		return -1;
}