#ifndef VW_MARKER_H_
#define VW_MARKER_H_

#include <vector>
#include "videowindow.h"
#include "AOITrapezium.h"
#include "ColorChooser.h"
#include "MotionDetector.h"

using namespace std;

typedef vector<AOIRect*>::iterator CR_Iterator;

class VW_Marker :
	public VideoWindow
{
protected:
	bool useRect_;
	bool cloneDone_;
	vector<AOIRect*> AOIs_;
	CR_Iterator currentAOI_;
	int drawStatus_;
	bool drawnOrChanged_;
	CvPoint lastMousePoint_;
	ColorChooser colorChooser_;
public:
	VW_Marker(int x,int y , int w, int h,
		const char* video=0, const char *L=0):VideoWindow(x,y,w,h,video,L) {
		useRect_ = true;
		cloneDone_ = true;
		drawStatus_ = NEW_RECT;
	}
	virtual ~VW_Marker(void);
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