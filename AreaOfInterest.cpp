#include "stdafx.h"
#include "AreaOfInterest.h"
#include "CvPoint_Wrapper.h"

AreaOfInterest::AreaOfInterest(void)
{
	cornerAreaPercentage_ = 0.0025;
	rect_ = cvRect(0, 0, 0, 0);
	color_ = cvScalar(0.0, 255.0, 0.0);
	colorOrg_ = NULL;
	thickness_ = 1;
  lineType_ = 20;
}

void AreaOfInterest::setRect(int x, int y, int w, int h) {
	rect_ = cvRect(x, y, w, h);
}

void AreaOfInterest::setRect(CvRect box) {
	rect_ = box;
}

void AreaOfInterest::setColor(double r, double g, double b) {
	color_ = cvScalar(r, g, b);
}

void AreaOfInterest::setColor(CvScalar color) {
	color_ = color;
}

CvRect AreaOfInterest::getRect() {
	return rect_;
}

CvScalar AreaOfInterest::getColor() {
	return color_;
}

CvPoint AreaOfInterest::getBottomRightCorner() {
	fixNegativeWH();
	return cvPoint(rect_.x + rect_.width, rect_.y + rect_.height);
}

int AreaOfInterest::getThickness() {
	return thickness_;
}

AreaOfInterest::~AreaOfInterest(void) {}

void AreaOfInterest::resize(int w, int h) {
	rect_.width = w; rect_.height = h;
}

void AreaOfInterest::resizeAddVector(CvPoint vector) {
	rect_.width += vector.x; rect_.height += vector.y;
}

bool AreaOfInterest::contains(CvPoint p) {
	fixNegativeWH();
	if (((rect_.x <= p.x) && (getBottomRightCorner().x >= p.x)) && 
		((rect_.y <= p.y) && (getBottomRightCorner().y >= p.y)))
		return true;
	return false;
}

void AreaOfInterest::fixNegativeWH() {
	if (rect_.width < 0) {
		rect_.x += rect_.width;
		rect_.width *= -1;
	}
	if (rect_.height < 0) {
		rect_.y += rect_.height;
		rect_.height *= -1;
	}
}

void AreaOfInterest::darkenColor() {
	if (colorOrg_ == NULL) {
		colorOrg_ = new CvScalar;
		*colorOrg_ = color_;
		double* val = color_.val;
		color_ = cvScalar(val[0]/2, val[1]/2, val[2]/2);
	}
}

void AreaOfInterest::returnOriginalColor() {
	if (colorOrg_ != NULL) {
		color_ = *colorOrg_;
		delete colorOrg_;
		colorOrg_ = NULL;
	}
}

void AreaOfInterest::move(CvPoint vector) {
	rect_.x += vector.x; rect_.y += vector.y;
}

int AreaOfInterest::actionController(CvPoint mousePointer) {
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

void AreaOfInterest::modify(int drawMethod, CvPoint vector) {
	CvPoint v_x, v_y;
	switch (drawMethod) {
  case MOVE_RECT:
    this->move(vector);
    break;
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
		throw "Exception here, modify to something meaningful please!";
	}
	//void fixNegativeWH();
}

void AreaOfInterest::drawSelfOnImage(IplImage* img) {
	cvRectangle(img, cvPoint(rect_.x, rect_.y),
		cvPoint(rect_.x+rect_.width, rect_.y+rect_.height),
    color_, thickness_);
}

bool AreaOfInterest::doesContainPoint(CvPoint p) {
  if (p.x < rect_.x)
    return false;
  if (p.x > rect_.x + rect_.width)
    return false;
  if (p.y < rect_.y)
    return false;
  if (p.y > rect_.y + rect_.height)
    return false;
  return true;
}