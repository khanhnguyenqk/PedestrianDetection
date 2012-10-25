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
	/*for (int i=0; i<AOIs_.size(); i++) {
		delete AOIs_[i];
	}*/
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

// Draw rect
void VW_Marker::drawAOI(IplImage* img,  AOIRect cr) {
	cvRectangle(img, cvPoint(cr.getRect().x, cr.getRect().y),
		cvPoint(cr.getRect().x+cr.getRect().width, cr.getRect().y+cr.getRect().height), 
		cr.getColor(), cr.getThickness());
}

void VW_Marker::drawAOI(IplImage* img, AOITrapezium ct) {
	int lineType = 20;
	CvScalar color = ct.getColor();
	int thickness = ct.getThickness();
	cvRectangle(img, cvPoint(ct.getRect().x, ct.getRect().y),
		cvPoint(ct.getRect().x+ct.getRect().width, ct.getRect().y+ct.getRect().height), 
		color, thickness);
	cvLine(img, fromCvPoint2D32f(ct.getPoint(0)), fromCvPoint2D32f(ct.getPoint(1)), color, thickness, lineType);
	cvLine(img, fromCvPoint2D32f(ct.getPoint(1)), fromCvPoint2D32f(ct.getPoint(2)), color, thickness, lineType);
	cvLine(img, fromCvPoint2D32f(ct.getPoint(2)), fromCvPoint2D32f(ct.getPoint(3)), color, thickness, lineType);
	cvLine(img, fromCvPoint2D32f(ct.getPoint(3)), fromCvPoint2D32f(ct.getPoint(0)), color, thickness, lineType);
}

void VW_Marker::drawAllAOIs(IplImage* img) {
	CR_Iterator it = AOIs_.begin();
	for (;it != AOIs_.end(); it++)
		drawAOI(img, *it);
}

int VW_Marker::mouseDrawingAOIHandle(int event) {
	int x = getRelativeMouseX(Fl::event_x());
	int y = getRelativeMouseY(Fl::event_y());
	AOITrapezium cr;
	switch (event) {

	case FL_PUSH:
		drawnOrChanged_ = true;
		cr.setRect(cvRect(x, y, 0, 0));
		AOIs_.push_back(cr);
		currentRect_=AOIs_.end() - 1;
		currentRect_->setColor(colorChooser_.getAColor());
		cloneAndDrawRects();
		this->redraw();
		return 1;
	case FL_DRAG:
		currentRect_->resize(x - currentRect_->getRect().x, y - currentRect_->getRect().y);
		cloneAndDrawRects();
		this->redraw();
		return 1;
	case FL_RELEASE:
		currentRect_->fixNegativeWH();
		currentRect_->finishCreating();
		cloneAndDrawRects();
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
		cloneAndDrawRects();
		return 1;
	case FL_DRAG:
		vector = subVectors(mousePoint, lastMousePoint_);
		currentRect_->move(vector);
		lastMousePoint_ = mousePoint;
		cloneAndDrawRects();
		this->redraw();
		return 1;
	case FL_RELEASE:
		currentRect_->fixNegativeWH();
		cloneAndDrawRects();
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
		cloneAndDrawRects();
		return 1;
	case FL_DRAG:
		currentRect_->moveCorner(drawStatus_, vector);
		
		lastMousePoint_ = mousePoint;
		cloneAndDrawRects();
		this->redraw();
		return 1;
	case FL_RELEASE:
		currentRect_->fixNegativeWH();
		cloneAndDrawRects();
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
		CR_Iterator it = currentRect_;
		do {
			drawStatus_ = it->actionController(p);
			if (drawStatus_ != -1) {
				currentRect_ = it;
				return;
			}
			it++;
			if (it == AOIs_.end()) it = AOIs_.begin();
		} while (it != currentRect_);

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

bool VW_Marker::deleteCurrentRect() {
	if (AOIs_.empty())
		return true;
	AOIs_.erase(currentRect_);
	currentRect_ = AOIs_.begin();
	return true;
}

bool VW_Marker::deleteAllRect() {
	if (AOIs_.empty())
		return true;
	AOIs_.clear();
	currentRect_ = AOIs_.begin();
	return true;
}

bool VW_Marker::cloneAndDrawRects() {
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
		if (it != currentRect_) {
			it->darkenColor();
		} else {
			it->returnOriginalColor();
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

bool VW_Marker::nextRect() {
	if (AOIs_.empty())
		return false;
	currentRect_ ++;
	if (currentRect_ == AOIs_.end())
		currentRect_ = AOIs_.begin();
	return true;
}

bool VW_Marker::prevRect() {
	if (AOIs_.empty())
		return false;
	if (currentRect_ == AOIs_.begin())
		currentRect_ = AOIs_.end();
	currentRect_--;
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
				cloneAndDrawRects();
				drawImageOnMainWindow(clone_);
			}
		} else if ((playStatus_ == PAUSE)) {
			drawBlackScreen();
			if (clone_ == NULL)
				stop();
			else {
				if (cloneDone_) {
					cloneAndDrawRects();
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