#include "stdafx.h"
#include "VW_Marker.h"

VideoWindowMarker::~VideoWindowMarker(void)
{
	for (unsigned i=0; i<aois_.size(); i++) {
		delete aois_[i];
	}
	aois_.clear();
}

int VideoWindowMarker::handle(int event) {
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

void VideoWindowMarker::drawAllAOIs(IplImage* img) {
	CR_Iterator it = aois_.begin();
	for (;it != aois_.end(); it++)
		(*it)->drawSelfOnImage(img);
}

int VideoWindowMarker::mouseDrawingAOIHandle(int event) {
	int x = getRelativeMouseX(Fl::event_x());
	int y = getRelativeMouseY(Fl::event_y());
	AreaOfInterest *aoi = new AoiTrapezium;
	switch (event) {

	case FL_PUSH:
		drawnOrChanged_ = true;
		aoi->setRect(cvRect(x, y, 0, 0));
		aois_.push_back(aoi);
		currentAoi_=aois_.end() - 1;
		(*currentAoi_)->setColor(colorChooser_.getAColor());
		cloneAndDrawAOIs();
		this->redraw();
		return 1;
	case FL_DRAG:
		(*currentAoi_)->resize(x - (*currentAoi_)->getRect().x, y - (*currentAoi_)->getRect().y);
		cloneAndDrawAOIs();
		this->redraw();
		return 1;
	case FL_RELEASE:
		(*currentAoi_)->fixNegativeWH();
		(*currentAoi_)->reassignedCorners();
		cloneAndDrawAOIs();
		this->redraw();
		drawStatus_ = -1;
		return 1;
	default:
		return 0;
	}
}

int VideoWindowMarker::mouseMovingAOIHandle(int event) {
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
		(*currentAoi_)->move(vector);
		lastMousePoint_ = mousePoint;
		cloneAndDrawAOIs();
		this->redraw();
		return 1;
	case FL_RELEASE:
		(*currentAoi_)->fixNegativeWH();
		cloneAndDrawAOIs();
		this->redraw();
		drawStatus_ = -1;
		return 1;
	default:
		return 0;
	}
}

int VideoWindowMarker::mouseResizeAOIHandle(int event) {
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
		(*currentAoi_)->moveCorner(drawStatus_, vector);
		
		lastMousePoint_ = mousePoint;
		cloneAndDrawAOIs();
		this->redraw();
		return 1;
	case FL_RELEASE:
		(*currentAoi_)->fixNegativeWH();
		cloneAndDrawAOIs();
		this->redraw();
		drawStatus_ = -1;
		return 1;
	default:
		return 0;
	}
}

void VideoWindowMarker::chooseDrawAction(int xMouse, int yMouse) {
	if (drawStatus_ == -1) {
		if (aois_.empty()) {
			drawStatus_ = NEW_RECT;
			return;
		}
		CvPoint p = cvPoint(xMouse, yMouse);
		CR_Iterator it = currentAoi_;
		do {
			if (useRect_)
				drawStatus_ = (*it)->actionController(p);
			else
				drawStatus_ = ((AoiTrapezium*)*it)->actionController(p);
			if (drawStatus_ != -1) {
				currentAoi_ = it;
				return;
			}
			it++;
			if (it == aois_.end()) it = aois_.begin();
		} while (it != currentAoi_);

		if (drawStatus_ == -1)
			drawStatus_ = NEW_RECT;
	} else if (drawStatus_ < 0)
		drawStatus_ = NEW_RECT;
}

string VideoWindowMarker::getSaveDirectory(const char* fileName) {
	int pos;
	string ret = string(fileName);
	pos = ret.find_last_of("\\");
	ret.erase(0, pos+1);
	ret = string("captured_") + ret;
	return ret;
}

bool VideoWindowMarker::setDrawStatus(int status) {
	if (status==NEW_RECT) {
		drawStatus_ = status;
		return true;
	}
	return false;
}

bool VideoWindowMarker::deleteCurrentAOI() {
	if (aois_.empty())
		return true;
	delete *currentAoi_;
	aois_.erase(currentAoi_);
	currentAoi_ = aois_.begin();
	return true;
}

bool VideoWindowMarker::deleteAllAOIs() {
	if (aois_.empty())
		return true;
	for (unsigned i=0; i<aois_.size(); i++) {
		delete aois_[i];
	}
	aois_.clear();
	currentAoi_ = aois_.begin();
	return true;
}

bool VideoWindowMarker::cloneAndDrawAOIs() {
	if (currFrame_ == NULL)
		return false;
	darkenNonCurrent();
	if (clone_)
		cvReleaseImage(&clone_);
	clone_ = cvCloneImage(currFrame_);
	drawAllAOIs(clone_);
	return true;
}

void VideoWindowMarker::darkenNonCurrent() {
	CR_Iterator it = aois_.begin();
	for (;it != aois_.end(); it++) {
		if (it != currentAoi_) {
			(*it)->darkenColor();
		} else {
			(*it)->returnOriginalColor();
		}
	}
}

int VideoWindowMarker::getRelativeMouseX(int x) {
	double ox,x1;
	ox=xPanRatio_*(1.0+zoomRatio_); x1=ox-zoomRatio_; x1=(x1+1)/2;
	double ret = (((double)x - x1*this->w())*
		(currFrame_->width)/(this->w()))/zoomRatio_;
	return (int)ret;
}

int VideoWindowMarker::getRelativeMouseY(int y) {
	double oy,y1;
	oy=-yPanRatio_*(1.0+zoomRatio_); y1=oy+zoomRatio_; y1=(-y1+1)/2;
	double ret = (((double)y - y1*this->h())*
		(currFrame_->height)/(this->h()))/zoomRatio_;
	return (int)ret;
}

bool VideoWindowMarker::nextAOI() {
	if (aois_.empty())
		return false;
	currentAoi_ ++;
	if (currentAoi_ == aois_.end())
		currentAoi_ = aois_.begin();
	return true;
}

bool VideoWindowMarker::prevAOI() {
	if (aois_.empty())
		return false;
	if (currentAoi_ == aois_.begin())
		currentAoi_ = aois_.end();
	currentAoi_--;
	return true;
}


void VideoWindowMarker::draw() {
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

void VideoWindowMarker::useRectangle(bool b) {
	useRect_ = b;
}

bool VideoWindowMarker::isUsingRectangle() {
	return useRect_;
}