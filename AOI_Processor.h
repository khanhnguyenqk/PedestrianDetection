#pragma once
#include "vw_marker.h"
class AOI_ProcessorWindow :
	public VW_Marker
{
protected:
	// Cropped Area of Interest (AOI) from drawn box
	vector<MotionDetector*> motionDetectors_;
	vector<IplImage*> aois_;
	vector<char*> windowNames_;
	bool showSubwindows_;

	// Methods
	void motionDetectorManage(int size);
	void clearMotionDetectorsMemories();

	void subwindowManage(int num);
	void drawPictureOnSubwindows(vector<IplImage*> images);
	void releaseImageVector(vector<IplImage*> &images);
	void trackMotionAndIllustrate(vector<IplImage*> src, vector<IplImage*> &dst);
public:
	AOI_ProcessorWindow(int x,int y , int w, int h,
		const char* video=0, const char *L=0):VW_Marker(x,y,w,h,video,L) {

			showSubwindows_ = true;
	}
	virtual ~AOI_ProcessorWindow(void);
	virtual void draw();

	void saveMarkedImage(IplImage* image);
	bool saveScreen();
	vector<IplImage*> extractAOI(IplImage *image, vector<CaptureRect> rs);
	vector<IplImage*> extractAOI(IplImage *image, vector<CaptureTrapezium> trs);
	virtual int handle(int event);

// Utilities
public:
	static void setElements(IplImage **image, CvPoint from, CvPoint to, uchar v);
};