#ifndef VW_MARKER_H_
#define VW_MARKER_H_

#include <vector>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <direct.h>
#include <sys/stat.h>
#include "CvPoint_Wrapper.h"
#include "videowindow.h"
#include "AOITrapezium.h"
#include "ColorChooser.h"

using namespace std;

typedef vector<AreaOfInterest*>::iterator CR_Iterator;

class VideoWindowMarker :
	public VideoWindow
{
protected:
	bool useRect_;
	bool cloneDone_;
	vector<AreaOfInterest*> aois_;
	CR_Iterator currentAoi_;
	int drawStatus_;
	bool drawnOrChanged_;
	CvPoint lastMousePoint_;
	ColorChooser colorChooser_;
public:
	VideoWindowMarker(int x,int y , int w, int h,
		const char* video=0, const char *L=0):VideoWindow(x,y,w,h,video,L) {
		useRect_ = true;
		cloneDone_ = true;
		drawStatus_ = NEW_RECT;
	}
	virtual ~VideoWindowMarker(void);
	virtual int handle(int event);

// Draw rect
protected:
	void drawAllAOIs(IplImage* img);
	int mouseDrawingAOIHandle(int event);
	int mouseMovingAOIHandle(int event);
	int mouseResizeAOIHandle(int event);
	void chooseDrawAction(int xMouse, int yMouse);
	void darkenNonCurrent();
	int getRelativeMouseX(int x);
	int getRelativeMouseY(int y);

// Other
public:
	string getSaveDirectory(const char* fileName);
	bool setDrawStatus(int status);
	bool deleteCurrentAOI();
	bool deleteAllAOIs();
	bool cloneAndDrawAOIs();
	bool nextAOI();
	bool prevAOI();
	void useRectangle(bool b);
	bool isUsingRectangle();

// Play control extend
	virtual void draw();
};

#endif