#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <direct.h>
#include <sys/stat.h>
#include "VW_Marker.h"
#include "CvPoint_Wrapper.h"

VW_Marker::~VW_Marker(void)
{
	for (unsigned i=0; i<aois_.size(); i++) {
		cvReleaseImage(&aois_[i]);
	}
	aois_.clear();
	for (unsigned i=0; i<windowNames_.size(); i++) {
		delete[] windowNames_[i];
	}
	windowNames_.clear();
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
	/*printf("x: %d | X: %d\ty: %d | Y: %d\n", x, y, this->x(), this->y());*/
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
	/*printf("x: %d | X: %d\ty: %d | Y: %d\n", x, y, this->x(), this->y());*/
	switch (event) {

	case FL_PUSH:
		drawnOrChanged_ = true;
		lastMousePoint_ = mousePoint;
		cloneAndDrawRects();
		return 1;
	case FL_DRAG:
		/*vector.x = x - lastMousePoint_.x;
		vector.y = y - lastMousePoint_.y;*/
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
	CvPoint v_a, v_b;
	/*printf("x: %d | X: %d\ty: %d | Y: %d\n", x, y, this->x(), this->y());*/
	switch (event) {

	case FL_PUSH:
		drawnOrChanged_ = true;
		lastMousePoint_ = mousePoint;
		cloneAndDrawRects();
		return 1;
	case FL_DRAG:
		switch (drawStatus_) {
		case RESIZE_BR:
			currentRect_->resizeAddVector(vector);
			break;
		case RESIZE_TL:
			currentRect_->move(vector);
			vector = mulVector(vector, -1);
			currentRect_->resizeAddVector(vector);
			break;
		case RESIZE_TR:
			v_a = cvPoint(0, vector.y); v_b = cvPoint(vector.x, -vector.y);
			currentRect_->move(v_a); currentRect_->resizeAddVector(v_b);
			break;
		case RESIZE_BL:
			v_a = cvPoint(vector.x, 0); v_b = cvPoint(-vector.x, vector.y);
			currentRect_->move(v_a); currentRect_->resizeAddVector(v_b);
			break;
		default:
			throw "Exception here, change to something meaningful please!";
		}
		
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
		int size = captureRects_.size();
		vector<IplImage*> aois = extractROIRects(currFrame_, captureRects_, size);
		for (int i=0; i<size; i++)
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
	/*int ret = (int)(((double)x - (1-zoomRatio_)*this->w()/2 - xPanRatio_*(this->w()))*
				(currFrame_->width)/(this->w()))/zoomRatio_;*/
	return (int)ret;
}

int VW_Marker::getRelativeMouseY(int y) {
	double oy,y1;
	oy=-yPanRatio_*(1.0+zoomRatio_); y1=oy+zoomRatio_; y1=(-y1+1)/2;
	double ret = (((double)y - y1*this->h())*
		(currFrame_->height)/(this->h()))/zoomRatio_;
	/*int ret = (int)(((double)y - (1-zoomRatio_)*this->h()/2 - yPanRatio_*(this->h()))*
	(currFrame_->height)/(this->h()))/zoomRatio_;*/
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

vector<IplImage*> VW_Marker::extractROIRects(IplImage *image, vector<CaptureRect> rects, int &size) {
	vector<IplImage*> ret;
	int s = rects.size();
	if (size < s) s = size;
	else size = s;

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
				drawIplImage(clone_);
			}
		} else if ((playStatus_ == PAUSE)) {
			drawBlackScreen();
			if (clone_ == NULL)
				stop();
			else {
				if (cloneDone_) {
					cloneAndDrawRects();
					drawIplImage(clone_);
				}
				else {
					drawAllRects(currFrame_);
					drawIplImage(currFrame_);
				}
			}
		} else if (playStatus_ == STOP)
			drawBlackScreen();
		/*printf("%f\n", playSpeed_);*/
	}
	updateDependences();
}