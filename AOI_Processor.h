#pragma once
#include "vw_marker.h"
#include "MotionDetector.h"

class AoiProcessorWindow :
	public VideoWindowMarker
{
protected:
	// Cropped Area of Interest (AOI) from drawn box
	MotionDetector* wholeMotionDetector_;
	vector<MotionDetector*> motionDetectors_;
	vector<IplImage*> aois_;
	vector<char*> windowNames_;
	bool extract_;

	// Methods
	void motionDetectorManage(int size);
	void clearMotionDetectorsMemories();

	void subwindowManage(int num);
	void drawPictureOnSubwindows(vector<IplImage*> images);
	void releaseImageVector(vector<IplImage*> &images);
	void trackMotionAndIllustrate(vector<IplImage*> src, vector<IplImage*> &dst);
public:
	AoiProcessorWindow(int x,int y , int w, int h,
		const char* video=0, const char *L=0):VideoWindowMarker(x,y,w,h,video,L) {
			extract_ = false;
			wholeMotionDetector_ = NULL;
	}
	virtual ~AoiProcessorWindow(void);
	virtual void draw();

	void saveMarkedImage(IplImage* image);
	bool saveScreen();
	vector<IplImage*> extractAOI(IplImage *image, vector<AreaOfInterest*> rs);
	virtual int handle(int event);

// Utilities
public:
	static void setElements(IplImage **image, CvPoint from, CvPoint to, uchar v);
};