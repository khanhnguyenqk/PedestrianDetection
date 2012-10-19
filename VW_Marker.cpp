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
	captureRects_.clear();
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
		} else if (Fl::event_button() == FL_MIDDLE_MOUSE) {
			saveScreen();
			return 1;
		}else if (Fl::event_button() == FL_LEFT_MOUSE) {
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
					r = mouseDrawingRectHandle(event);
					break;
				case MOVE_RECT:
					r = mouseMovingRectHandle(event);
					break;
				case RESIZE_BR:
				case RESIZE_TL:
				case RESIZE_TR:
				case RESIZE_BL:
					r = mouseResizeRectHandle(event);
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
void VW_Marker::drawRect(IplImage* img,  CaptureRect cr) {
	cvRectangle(img, cvPoint(cr.getRect().x, cr.getRect().y),
		cvPoint(cr.getRect().x+cr.getRect().width, cr.getRect().y+cr.getRect().height), 
		cr.getColor(), cr.getThickness());
}

void VW_Marker::drawAllRects(IplImage* img) {
	CR_Iterator it = captureRects_.begin();
	for (;it != captureRects_.end(); it++)
		drawRect(img, *it);
}

int VW_Marker::mouseDrawingRectHandle(int event) {
	int x = getRelativeMouseX(Fl::event_x());
	int y = getRelativeMouseY(Fl::event_y());
	CaptureRect cr;
	switch (event) {

	case FL_PUSH:
		drawnOrChanged_ = true;
		cr.setRect(cvRect(x, y, 0, 0));
		captureRects_.push_back(cr);
		currentRect_=captureRects_.end() - 1;
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
		cloneAndDrawRects();
		this->redraw();
		drawStatus_ = -1;
		return 1;
	default:
		return 0;
	}
}

int VW_Marker::mouseMovingRectHandle(int event) {
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

int VW_Marker::mouseResizeRectHandle(int event) {
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
		if (captureRects_.empty()) {
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
			if (it == captureRects_.end()) it = captureRects_.begin();
		} while (it != currentRect_);

		if (drawStatus_ == -1)
			drawStatus_ = NEW_RECT;
	} else if (drawStatus_ < 0)
		drawStatus_ = NEW_RECT;
}

void VW_Marker::saveMarkedImage(IplImage* image) {
	string directoryName = VW_Marker::getSaveDirectory(videoName_);
	printf("%s", directoryName.c_str());
	bool diretoryExists = false;

	struct stat st;
	if (stat(directoryName.c_str(), &st) == 0)
		diretoryExists = true;
	else
	{
		int mkdirResult = _mkdir(directoryName.c_str());
		if (mkdirResult == 0)
			diretoryExists = true;
		else
		{
			char errMsg[256];
			sprintf(errMsg, "The directory creation failed with error: %d. Cannot save image\n", mkdirResult);
			fl_alert(errMsg);
		}
	}

	if (diretoryExists) {
		int frameNum = (int)cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES);
		char filePath[512];
		sprintf(filePath, "%s\\%010d.jpg", directoryName.c_str(), frameNum);
		cvSaveImage(filePath, image);
		// Save rects areas
		vector<IplImage*> aois = extractROIRects(currFrame_, captureRects_);
		for (unsigned i=0; i<aois.size(); i++)
		{
			sprintf(filePath, "%s\\%010d_%05d.jpg", directoryName.c_str(), frameNum, i);
			cvSaveImage(filePath, aois[i]);
			cvReleaseImage(&aois[i]);
		}
		aois.clear();
	}
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
	if (captureRects_.empty())
		return true;
	captureRects_.erase(currentRect_);
	currentRect_ = captureRects_.begin();
	return true;
}

bool VW_Marker::deleteAllRect() {
	if (captureRects_.empty())
		return true;
	captureRects_.clear();
	currentRect_ = captureRects_.begin();
	return true;
}

bool VW_Marker::cloneAndDrawRects() {
	if (currFrame_ == NULL)
		return false;
	darkenNonCurrent();
	if (clone_)
		cvReleaseImage(&clone_);
	clone_ = cvCloneImage(currFrame_);
	drawAllRects(clone_);
	return true;
}

void VW_Marker::darkenNonCurrent() {
	CR_Iterator it = captureRects_.begin();
	for (;it != captureRects_.end(); it++) {
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
	if (captureRects_.empty())
		return false;
	currentRect_ ++;
	if (currentRect_ == captureRects_.end())
		currentRect_ = captureRects_.begin();
	return true;
}

bool VW_Marker::prevRect() {
	if (captureRects_.empty())
		return false;
	if (currentRect_ == captureRects_.begin())
		currentRect_ = captureRects_.end();
	currentRect_--;
	return true;
}

bool VW_Marker::saveScreen() {
	if ((clone_ == NULL) && (currFrame_ == NULL))
		return false;
	if (playStatus_==PAUSE) {
		if (clone_)
			saveMarkedImage(clone_);
		else
			return false;
	}
	else if (playStatus_==PLAY) {
		if (currFrame_)
			saveMarkedImage(currFrame_);
		else
			return false;
	}
	else
		return false;
	return true;
}

vector<IplImage*> VW_Marker::extractROIRects(IplImage *image, vector<CaptureRect> rects) {
	vector<IplImage*> ret;
	int s = rects.size();

	for (int i = 0; i<s; i++) {
		cvSetImageROI(image, rects[i].getRect());
		ret.push_back(cvCreateImage(cvGetSize(image), image->depth, image->nChannels));
		cvCopy(image, ret[i], NULL);
		cvResetImageROI(image);
	}

	return ret;
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
					drawAllRects(currFrame_);
					drawImageOnMainWindow(currFrame_);
				}
			}
		} else if (playStatus_ == STOP)
			drawBlackScreen();
	}
	updateDependences();
}