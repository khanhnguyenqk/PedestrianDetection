#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <direct.h>
#include <sys/stat.h>
#include "VW_Marker.h"
#include "CvPoint_Wrapper.h"
#include "MotionDetector.h"

VW_Marker::~VW_Marker(void)
{
	for (unsigned i=0; i<AOIs_.size(); i++) {
		delete AOIs_[i];
	}
	AOIs_.clear();
}

int VW_Marker::handle(int event) {
	if (!videoInitiated_)
		return VideoWindow::handle(event);
	int x;
	int y;
	switch (event) {
	case FL_PUSH:
		if (Fl::event_button() == FL_RIGHT_MOUSE) {
			showNextFrame();
			return 1;
		} else if (Fl::event_button() == FL_LEFT_MOUSE) {
			x = getRelativeMouseX(Fl::event_x());
			y = getRelativeMouseY(Fl::event_y());
			chooseDrawAction(x, y);
		}
	case FL_DRAG:
		if (Fl::event_button() == FL_RIGHT_MOUSE) {
			showNextFrame();
			return 1;
		}
	case FL_RELEASE:
		if (Fl::event_button() == FL_LEFT_MOUSE) {
			if (playStatus_ == PAUSE) {
				int r;
				cloneDone_ = false;
				switch (drawStatus_) {
				case NEW_RECT:
					r = mouseDrawingAOIHandle(event);
					break;
				case MOVE_RECT:
					r = mouseMovingAOIHandle(event);
					break;
				case RESIZE_BR:
				case RESIZE_TL:
				case RESIZE_TR:
				case RESIZE_BL:
				case P0: case P1: case P2: case P3:
					r = mouseResizeAOIHandle(event);
					break;
				default:
					return 0;
				}
				cloneDone_ = true;
				return r;
			}
		}
	}

	return VideoWindow::handle(event);
}

void VW_Marker::drawAllAOIs(IplImage* img) {
	CR_Iterator it = AOIs_.begin();
	for (;it != AOIs_.end(); it++)
		(*it)->drawSelfOnImage(img);
}

int VW_Marker::mouseDrawingAOIHandle(int event) {
	int x = getRelativeMouseX(Fl::event_x());
	int y = getRelativeMouseY(Fl::event_y());
	AOIRect *aoi = new AOITrapezium;
	switch (event) {

	case FL_PUSH:
		drawnOrChanged_ = true;
		aoi->setRect(cvRect(x, y, 0, 0));
		AOIs_.push_back(aoi);
		currentAOI_=AOIs_.end() - 1;
		(*currentAOI_)->setColor(colorChooser_.getAColor());
		cloneAndDrawAOIs();
		this->redraw();
		return 1;
	case FL_DRAG:
		(*currentAOI_)->resize(x - (*currentAOI_)->getRect().x, y - (*currentAOI_)->getRect().y);
		cloneAndDrawAOIs();
		this->redraw();
		return 1;
	case FL_RELEASE:
		(*currentAOI_)->fixNegativeWH();
		(*currentAOI_)->reassignedCorners();
		cloneAndDrawAOIs();
		this->redraw();
		drawStatus_ = -1;
		return 1;
	default:
		return 0;
	}
}

int VW_Marker::mouseMovingAOIHandle(int event) {
	int x = getRelativeMouseX(Fl::event_x());
	int y = getRelativeMouseY(Fl::event_y());
	CvPoint mousePoint = cvPoint(x, y);
	CvPoint vector;
	switch (event) {

	case FL_PUSH:
		drawnOrChanged_ = true;
		lastMousePoint_ = mousePoint;
		cloneAndDrawAOIs();
		return 1;
	case FL_DRAG:
		vector = subVectors(mousePoint, lastMousePoint_);
		(*currentAOI_)->move(vector);
		lastMousePoint_ = mousePoint;
		cloneAndDrawAOIs();
		this->redraw();
		return 1;
	case FL_RELEASE:
		(*currentAOI_)->fixNegativeWH();
		cloneAndDrawAOIs();
		this->redraw();
		drawStatus_ = -1;
		return 1;
	default:
		return 0;
	}
}

int VW_Marker::mouseResizeAOIHandle(int event) {
	int x = getRelativeMouseX(Fl::event_x());
	int y = getRelativeMouseY(Fl::event_y());
	CvPoint mousePoint = cvPoint(x, y);
	CvPoint vector = subVectors(mousePoint, lastMousePoint_);
	
	switch (event) {

	case FL_PUSH:
		drawnOrChanged_ = true;
		lastMousePoint_ = mousePoint;
		cloneAndDrawAOIs();
		return 1;
	case FL_DRAG:
		(*currentAOI_)->moveCorner(drawStatus_, vector);
		
		lastMousePoint_ = mousePoint;
		cloneAndDrawAOIs();
		this->redraw();
		return 1;
	case FL_RELEASE:
		(*currentAOI_)->fixNegativeWH();
		cloneAndDrawAOIs();
		this->redraw();
		drawStatus_ = -1;
		return 1;
	default:
		return 0;
	}
}

void VW_Marker::chooseDrawAction(int xMouse, int yMouse) {
	if (drawStatus_ == -1) {
		if (AOIs_.empty()) {
			drawStatus_ = NEW_RECT;
			return;
		}
		CvPoint p = cvPoint(xMouse, yMouse);
		CR_Iterator it = currentAOI_;
		do {
			if (useRect_)
				drawStatus_ = (*it)->actionController(p);
			else
				drawStatus_ = ((AOITrapezium*)*it)->actionController(p);
			if (drawStatus_ != -1) {
				currentAOI_ = it;
				return;
			}
			it++;
			if (it == AOIs_.end()) it = AOIs_.begin();
		} while (it != currentAOI_);

		if (drawStatus_ == -1)
			drawStatus_ = NEW_RECT;
	} else if (drawStatus_ < 0)
		drawStatus_ = NEW_RECT;
}

string VW_Marker::getSaveDirectory(const char* fileName) {
	int pos;
	string ret = string(fileName);
	pos = ret.find_last_of("\\");
	ret.erase(0, pos+1);
	ret = string("captured_") + ret;
	return ret;
}

bool VW_Marker::setDrawStatus(int status) {
	if (status==NEW_RECT) {
		drawStatus_ = status;
		return true;
	}
	return false;
}

bool VW_Marker::deleteCurrentAOI() {
	if (AOIs_.empty())
		return true;
	delete *currentAOI_;
	AOIs_.erase(currentAOI_);
	currentAOI_ = AOIs_.begin();
	return true;
}

bool VW_Marker::deleteAllAOIs() {
	if (AOIs_.empty())
		return true;
	for (unsigned i=0; i<AOIs_.size(); i++) {
		delete AOIs_[i];
	}
	AOIs_.clear();
	currentAOI_ = AOIs_.begin();
	return true;
}

bool VW_Marker::cloneAndDrawAOIs() {
	if (currFrame_ == NULL)
		return false;
	darkenNonCurrent();
	if (clone_)
		cvReleaseImage(&clone_);
	clone_ = cvCloneImage(currFrame_);
	drawAllAOIs(clone_);
	return true;
}

void VW_Marker::darkenNonCurrent() {
	CR_Iterator it = AOIs_.begin();
	for (;it != AOIs_.end(); it++) {
		if (it != currentAOI_) {
			(*it)->darkenColor();
		} else {
			(*it)->returnOriginalColor();
		}
	}
}

int VW_Marker::getRelativeMouseX(int x) {
	double ox,x1;
	ox=xPanRatio_*(1.0+zoomRatio_); x1=ox-zoomRatio_; x1=(x1+1)/2;
	double ret = (((double)x - x1*this->w())*
		(currFrame_->width)/(this->w()))/zoomRatio_;
	return (int)ret;
}

int VW_Marker::getRelativeMouseY(int y) {
	double oy,y1;
	oy=-yPanRatio_*(1.0+zoomRatio_); y1=oy+zoomRatio_; y1=(-y1+1)/2;
	double ret = (((double)y - y1*this->h())*
		(currFrame_->height)/(this->h()))/zoomRatio_;
	return (int)ret;
}

bool VW_Marker::nextAOI() {
	if (AOIs_.empty())
		return false;
	currentAOI_ ++;
	if (currentAOI_ == AOIs_.end())
		currentAOI_ = AOIs_.begin();
	return true;
}

bool VW_Marker::prevAOI() {
	if (AOIs_.empty())
		return false;
	if (currentAOI_ == AOIs_.begin())
		currentAOI_ = AOIs_.end();
	currentAOI_--;
	return true;
}


void VW_Marker::draw() {
	if (videoInitiated_) {
		if ((playStatus_ == PLAY)) {
			currFrame_ = cvQueryFrame(videoCapture_);

			drawBlackScreen();
			if (currFrame_ == NULL)
				stop();
			else {
				cloneAndDrawAOIs();
				drawImageOnMainWindow(clone_);
			}
		} else if ((playStatus_ == PAUSE)) {
			drawBlackScreen();
			if (clone_ == NULL)
				stop();
			else {
				if (cloneDone_) {
					cloneAndDrawAOIs();
					drawImageOnMainWindow(clone_);
				}
				else {
					drawAllAOIs(currFrame_);
					drawImageOnMainWindow(currFrame_);
				}
			}
		} else if (playStatus_ == STOP)
			drawBlackScreen();
	}
	updateDependences();
}

void VW_Marker::useRectangle(bool b) {
	useRect_ = b;
}

bool VW_Marker::isUsingRectangle() {
	return useRect_;
}