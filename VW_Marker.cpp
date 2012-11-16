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
			chooseDrawAction(cvPoint(x, y));
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
        if (!new0_modify1_) {
          r = mouseCreateToolHandle(event);
        } else {
          r = mouseModifyToolHandle(event);
        }
        
				cloneDone_ = true;
				return r;
			}
		}
	}

	return VideoWindow::handle(event);
}

void VideoWindowMarker::drawAllTools(IplImage* img) {
  for (int i=0; i<aois_.size(); i++)
    aois_[i]->drawSelfOnImage(clone_);
  for (int i=0; i< lines_.size(); i++)
    lines_[i]->drawSelfOnImage(clone_);
}

int VideoWindowMarker::mouseCreateToolHandle(int event) {
	int x = getRelativeMouseX(Fl::event_x());
	int y = getRelativeMouseY(Fl::event_y());
  CvPoint mousePoint = cvPoint(x, y);
  CvPoint vector = subVectors(mousePoint, lastMousePoint_);;

  AreaOfInterest *aoi;
  DrawToolLine *line;
  if (!aoi0_line1_)
	  aoi = new AoiTrapezium;
  else if (aoi0_line1_ == 1)
    line = new DrawToolLine;

	switch (event) {
	case FL_PUSH:
		drawnOrChanged_ = true;

    if (!aoi0_line1_) {
      aoi->setRect(cvRect(x, y, 0, 0));
      aois_.push_back(aoi);
      currentAoi_=aois_.end() - 1;
      (*currentAoi_)->setColor(colorChooser_.getAColor());
    } else if (aoi0_line1_ == 1) {
      line->setLine(cvPoint(x, y), cvPoint(x, y));
      line->setColor(colorChooser_.getAColor());
      lines_.push_back(line);
      currentLine_=lines_.end() - 1;
    }
    lastMousePoint_ = mousePoint;
		cloneAndDrawTools();
		this->redraw();
		return 1;
	case FL_DRAG:
    if (!aoi0_line1_) {
      (*currentAoi_)->resize(x - (*currentAoi_)->getRect().x, y - (*currentAoi_)->getRect().y);
    } else if (aoi0_line1_ == 1) {
      (*currentLine_)->modify(LINE_P2, vector);
    }
    lastMousePoint_ = mousePoint;
		cloneAndDrawTools();
		this->redraw();
		return 1;
	case FL_RELEASE:
    if (!aoi0_line1_) {
      (*currentAoi_)->fixNegativeWH();
      (*currentAoi_)->reassignedCorners();
    }
		cloneAndDrawTools();
		this->redraw();
		new0_modify1_ = -1;
		return 1;
	default:
		return 0;
	}
}

int VideoWindowMarker::mouseModifyToolHandle(int event) {
	int x = getRelativeMouseX(Fl::event_x());
	int y = getRelativeMouseY(Fl::event_y());
	CvPoint mousePoint = cvPoint(x, y);
	CvPoint vector = subVectors(mousePoint, lastMousePoint_);;

	switch (event) {
	case FL_PUSH:
		drawnOrChanged_ = true;
		lastMousePoint_ = mousePoint;
		cloneAndDrawTools();
		return 1;
	case FL_DRAG:
    if (!aoi0_line1_)
		  (*currentAoi_)->modify(modifyCodeNum_, vector);
    else if (aoi0_line1_ == 1)
      (*currentLine_)->modify(modifyCodeNum_, vector);
		lastMousePoint_ = mousePoint;
		cloneAndDrawTools();
		this->redraw();
		return 1;
	case FL_RELEASE:
    if (!aoi0_line1_)
		  (*currentAoi_)->fixNegativeWH();
		cloneAndDrawTools();
		this->redraw();
		new0_modify1_ = -1;
		return 1;
	default:
		return 0;
	}
}

void VideoWindowMarker::chooseDrawAction( CvPoint mouse )
{
	if (new0_modify1_ == -1) {
    new0_modify1_= 1;
		if (aois_.empty() && lines_.empty()) {
      markNewToolFlag();
			return;
		}

    // Check on lines first because lines are smaller than aois
    if (!lines_.empty()) {
      vector<DrawToolLine*>::iterator itLine = currentLine_;
      do {
        modifyCodeNum_ = (*itLine)->actionController(mouse);
        if (modifyCodeNum_ != -1) {
          currentLine_ = itLine;
          return;
        }
        itLine++;
        if (itLine == lines_.end()) itLine = lines_.begin();
      } while (itLine != currentLine_);
    }

    // Check on all aois
    if (!aois_.empty()) {
      vector<AreaOfInterest*>::iterator itAoi = currentAoi_;
      do {
        if (useRect_)
          modifyCodeNum_ = (*itAoi)->actionController(mouse);
        else
          modifyCodeNum_ = ((AoiTrapezium*)*itAoi)->actionController(mouse);
        if (modifyCodeNum_ != -1) {
          currentAoi_ = itAoi;
          return;
        }
        itAoi++;
        if (itAoi == aois_.end()) itAoi = aois_.begin();
      } while (itAoi != currentAoi_);
    }

		if (modifyCodeNum_ == -1)
			markNewToolFlag();
	}
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
	if (!status) {
		new0_modify1_ = status;
		return true;
	}
	return false;
}

bool VideoWindowMarker::deleteCurrentTool() {
	if (aois_.empty())
		return true;
	delete *currentAoi_;
	aois_.erase(currentAoi_);
	currentAoi_ = aois_.begin();
	return true;
}

bool VideoWindowMarker::deleteAllTools() {
	if (aois_.empty())
		return true;
	for (unsigned i=0; i<aois_.size(); i++) {
		delete aois_[i];
	}
	aois_.clear();
	currentAoi_ = aois_.begin();
	return true;
}

bool VideoWindowMarker::cloneAndDrawTools() {
	if (currFrame_ == NULL)
		return false;
  // Darken all & Return the current aoi and line to original color
  if (!aois_.empty()) {
    for (int i=0; i<aois_.size(); i++)
      aois_[i]->darkenColor();
    (*currentAoi_)->returnOriginalColor();
  }
	
  if (!lines_.empty()) {
    for (int i=0; i< lines_.size(); i++)
      lines_[i]->darkenColor();
    (*currentLine_)->returnOriginalColor();
  }

	if (clone_)
		cvReleaseImage(&clone_);
	clone_ = cvCloneImage(currFrame_);

	// Draw everything on the picture
  drawAllTools(clone_);

	return true;
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
				cloneAndDrawTools();
				drawImageOnMainWindow(clone_);
			}
		} else if ((playStatus_ == PAUSE)) {
			drawBlackScreen();
			if (clone_ == NULL)
				stop();
			else {
				if (cloneDone_) {
					cloneAndDrawTools();
					drawImageOnMainWindow(clone_);
				}
				else {
					drawAllTools(currFrame_);
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

void VideoWindowMarker::markNewToolFlag()
{
  new0_modify1_ = 0;
}
