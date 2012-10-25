#include "stdafx.h"
#include "AOIRect.h"
#include "CvPoint_Wrapper.h"

AOIRect::AOIRect(void)
{
	cornerAreaPercentage_ = 0.0025;
	rect_ = cvRect(0, 0, 0, 0);
	color_ = cvScalar(0.0, 255.0, 0.0);
	colorOrg_ = NULL;
	thickness_ = 1;
}

void AOIRect::setRect(int x, int y, int w, int h) {
	rect_ = cvRect(x, y, w, h);
}

void AOIRect::setRect(CvRect box) {
	rect_ = box;
}

void AOIRect::setColor(double r, double g, double b) {
	color_ = cvScalar(r, g, b);
}

void AOIRect::setColor(CvScalar color) {
	color_ = color;
}

CvRect AOIRect::getRect() {
	return rect_;
}

CvScalar AOIRect::getColor() {
	return color_;
}

CvPoint AOIRect::getBottomRightCorner() {
	fixNegativeWH();
	return cvPoint(rect_.x + rect_.width, rect_.y + rect_.height);
}

int AOIRect::getThickness() {
	return thickness_;
}

AOIRect::~AOIRect(void) {}

void AOIRect::resize(int w, int h) {
	rect_.width = w; rect_.height = h;
}

void AOIRect::resizeAddVector(CvPoint vector) {
	rect_.width += vector.x; rect_.height += vector.y;
}

bool AOIRect::contains(CvPoint p) {
	fixNegativeWH();
	if (((rect_.x <= p.x) && (getBottomRightCorner().x >= p.x)) && 
		((rect_.y <= p.y) && (getBottomRightCorner().y >= p.y)))
		return true;
	return false;
}

void AOIRect::fixNegativeWH() {
	if (rect_.width < 0) {
		rect_.x += rect_.width;
		rect_.width *= -1;
	}
	if (rect_.height < 0) {
		rect_.y += rect_.height;
		rect_.height *= -1;
	}
}

void AOIRect::darkenColor() {
	if (colorOrg_ == NULL) {
		colorOrg_ = new CvScalar;
		*colorOrg_ = color_;
		double* val = color_.val;
		color_ = cvScalar(val[0]/2, val[1]/2, val[2]/2);
	}
}

void AOIRect::returnOriginalColor() {
	if (colorOrg_ != NULL) {
		color_ = *colorOrg_;
		delete colorOrg_;
		colorOrg_ = NULL;
	}
}

void AOIRect::move(CvPoint vector) {
	rect_.x += vector.x; rect_.y += vector.y;
}

int AOIRect::actionController(CvPoint mousePointer) {
	if (this->contains(mousePointer)) {
		double sqrtP = sqrt(cornerAreaPercentage_);
		CvPoint tl = cvPoint(rect_.x,rect_.y);
		CvPoint tr = cvPoint(rect_.x+rect_.width,rect_.y);
		CvPoint sizeVector = cvPoint(rect_.width, rect_.height);
		CvPoint sizeVectorInvert = cvPoint(-rect_.width, rect_.height);
		/*CvPoint center = divVector(addVectors(sizeVector,tl), 2);*/

		CvPoint tl_inner = addVectors(tl, mulVector(sizeVector, sqrtP));
		if ((mousePointer.x <= tl_inner.x) && (mousePointer.y <= tl_inner.y))
			return RESIZE_TL;

		CvPoint tr_inner = addVectors(tr, mulVector(sizeVectorInvert, sqrtP));
		if ((mousePointer.x >= tr_inner.x) && (mousePointer.y <= tr_inner.y))
			return RESIZE_TR;

		CvPoint br_inner = addVectors(tl, mulVector(sizeVector, 1 - sqrtP));
		if ((mousePointer.x >= br_inner.x) && (mousePointer.y >= br_inner.y))
			return RESIZE_BR;

		CvPoint bl_inner = addVectors(tr, mulVector(sizeVectorInvert, 1 - sqrtP));
		if ((mousePointer.x <= bl_inner.x) && (mousePointer.y >= bl_inner.y))
			return RESIZE_BL;

		return MOVE_RECT;
	}
	else
		return -1;
}

void AOIRect::moveCorner(int drawMethod, CvPoint vector) {
	CvPoint v_x, v_y;
	switch (drawMethod) {
	case RESIZE_BR:
		this->resizeAddVector(vector);
		break;
	case RESIZE_TL:
		this->move(vector);
		vector = mulVector(vector, -1);
		this->resizeAddVector(vector);
		break;
	case RESIZE_TR:
		v_x = cvPoint(0, vector.y); v_y = cvPoint(vector.x, -vector.y);
		this->move(v_x); this->resizeAddVector(v_y);
		break;
	case RESIZE_BL:
		v_x = cvPoint(vector.x, 0); v_y = cvPoint(-vector.x, vector.y);
		this->move(v_x); this->resizeAddVector(v_y);
		break;
	default:
		throw "Exception here, change to something meaningful please!";
	}
}