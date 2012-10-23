#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <direct.h>
#include <sys/stat.h>
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

int AOI_ProcessorWindow::handle(int event) {
	if (!videoInitiated_)
		return VideoWindow::handle(event);

	if (event == FL_PUSH) {
		if (Fl::event_button() == FL_MIDDLE_MOUSE) {
			saveScreen();
			return 1;
		}
	}
	return VW_Marker::handle(event);
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

bool AOI_ProcessorWindow::saveScreen() {
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

vector<IplImage*> AOI_ProcessorWindow::extractROIRects(IplImage *image, vector<CaptureRect> rs) {
	vector<IplImage*> ret;
	int s = rs.size();

	for (int i = 0; i<s; i++) {
		cvSetImageROI(image, rs[i].getRect());
		ret.push_back(cvCreateImage(cvGetSize(image), image->depth, image->nChannels));
		cvCopy(image, ret[i], NULL);
		cvResetImageROI(image);
	}

	return ret;
}

vector<IplImage*> AOI_ProcessorWindow::extractROIRects(IplImage *image, vector<CaptureTrapezium> ts) {
	vector<IplImage*> ret;
	int s = ts.size();

	for (int i = 0; i<s; i++) {
		cvSetImageROI(image, ts[i].getRect());
		ret.push_back(cvCreateImage(cvGetSize(image), image->depth, image->nChannels));
		cvCopy(image, ret[i], NULL);
		setElements(&ret[i], cvPoint(0,20), cvPoint(50, 22), (uchar)0);
		cvResetImageROI(image);
	}

	return ret;
}

void AOI_ProcessorWindow::saveMarkedImage(IplImage* image) {
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
		// Save ts areas
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

void AOI_ProcessorWindow::setElements(IplImage **image, CvPoint from, CvPoint to, uchar v) {
	// Check some conditions
	if (from.x == to.x && from.y == to.y)
		return;
	if (!(from.x <= to.x && from.y <= to.y))
		return;
	int w = (*image)->width;
	int h = (*image)->height;
	if (!((from.x < w) && (from.y < h) && (to.x < w) && (to.y < h)))
		return;
	
	// Pass all conditions.
	for (int y = from.y; y <= to.y; y++) {
		uchar *ptr;
		int begin, end;
		if (y == from.y) {
			ptr = (uchar*)((*image)->imageData + y * (*image)->widthStep + from.x);
			begin = from.x;
		} else {
			ptr = (uchar*)((*image)->imageData + y * (*image)->widthStep);
			begin = 0;
		}
		if (y == to.y) {
			end = to.x;
		} else {
			end = (*image)->width - 1;
		}
		for (int x = begin; x <= end; x++) {
			ptr[3*x] = v;
			ptr[3*x + 1] = v;
			ptr[3*x + 2] = v;
		}
	}
}