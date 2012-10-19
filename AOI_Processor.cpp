#include "StdAfx.h"
#include "AOI_Processor.h"

AOI_ProcessorWindow::~AOI_ProcessorWindow(void)
{
	releaseImageVector(aois_);

	for (unsigned i=0; i<windowNames_.size(); i++) {
		delete[] windowNames_[i];
	}
	windowNames_.clear();
	cvDestroyAllWindows();
}

void AOI_ProcessorWindow::subwindowManage(int num) {
	int numWindows = windowNames_.size();
	// Create more if needed
	if (num > numWindows) {
		int i = numWindows;
		for (;i < num; i++) {
			char *name;
			name = new char[128];
			sprintf(name, "Window %05d", i);
			windowNames_.push_back(name);
			cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
		}
	}
	// Destroy if have to
	else if (num < numWindows) {
		int i = num;
		for (;i < numWindows; i++) {
			cvDestroyWindow(windowNames_[i]);
			delete[] windowNames_[i];
		}
		windowNames_.erase(windowNames_.begin()+num, windowNames_.end());
	}
}

void AOI_ProcessorWindow::motionDetectorManage(int size) {
	int numDetector = motionDetectors_.size();
	
	// If changed, remove all. Create brand new.
	if (size != numDetector) {
		clearMotionDetectorsMemories();
		// Create more if needed
		for (int i=0; i < size; i++) {
			MotionDetector *m = new MotionDetector(cvSize(aois_[i]->width, aois_[i]->height));
			motionDetectors_.push_back(m);
		}
	}
}

void AOI_ProcessorWindow::drawPictureOnSubwindows(vector<IplImage*> images) {
	subwindowManage(images.size());
	if (images.size() != windowNames_.size()) {
		fl_alert("Number of cropped areas does not match number on windows created.");
		return;
	}
	for (unsigned i=0; i<images.size(); i++) {
		cvShowImage(windowNames_[i], images[i]);
	}
}

void AOI_ProcessorWindow::releaseImageVector(vector<IplImage*> &images) {
	for (unsigned i=0; i<images.size(); i++) {
		cvReleaseImage(&images[i]);
	}
	images.clear();
}

void AOI_ProcessorWindow::draw() {
	if (videoInitiated_) {
		if ((playStatus_ == PLAY)) {
			currFrame_ = cvQueryFrame(videoCapture_);

			drawBlackScreen();
			if (currFrame_ == NULL)
				stop();
			else {
				cloneAndDrawRects();
				drawImageOnMainWindow(clone_);
				if (showSubwindows_) {
					vector<IplImage*> motionTracked;
					aois_ = extractROIRects(currFrame_, captureRects_);
					trackMotionAndIllustrate(aois_, motionTracked);
					drawPictureOnSubwindows(motionTracked);
					releaseImageVector(aois_);
					releaseImageVector(motionTracked);
				}
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

void AOI_ProcessorWindow::clearMotionDetectorsMemories() {
	int numDetector = motionDetectors_.size();
	for (int i = 0; i<numDetector; i++) {
		delete motionDetectors_[i];
	}
	motionDetectors_.clear();
}

void AOI_ProcessorWindow::trackMotionAndIllustrate(vector<IplImage*> src, vector<IplImage*> &dst) 
{
	motionDetectorManage(src.size());
	for (unsigned i=0; i<src.size(); i++) {
		IplImage *img = motionDetectors_[i]->processImage(src[i]);
		dst.push_back(img);
	}
}