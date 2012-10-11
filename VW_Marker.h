#ifndef VW_MARKER_H_
#define VW_MARKER_H_

#include <vector>
#include "videowindow.h"
#include "CaptureRect.h"
#include "ColorChooser.h"
#include "MotionDetector.h"

using namespace std;

typedef vector<CaptureRect>::iterator CR_Iterator;

class VW_Marker :
	public VideoWindow
{
protected:
	bool cloneDone_;
	CaptureRect cr_;
	vector<CaptureRect> captureRects_;
	CR_Iterator currentRect_;
	int drawStatus_;
	bool drawnOrChanged_;
	CvPoint lastMousePoint_;
	ColorChooser colorChooser_;
	
public:
	VW_Marker(int x,int y , int w, int h,
		const char* video=0, const char *L=0):VideoWindow(x,y,w,h,video,L) {
		cloneDone_ = true;
		drawStatus_ = NEW_RECT;
	}
	virtual ~VW_Marker(void);
	virtual int handle(int event);

// Draw rect
protected:
	void drawRect(IplImage* img, CaptureRect cr);
	void drawAllRects(IplImage* img);
	int mouseDrawingRectHandle(int event);
	int mouseMovingRectHandle(int event);
	int mouseResizeRectHandle(int event);
	void chooseDrawAction(int xMouse, int yMouse);
	void saveMarkedImage(IplImage* image);
	void darkenNonCurrent();
	int getRelativeMouseX(int x);
	int getRelativeMouseY(int y);

// Other
public:
	string getSaveDirectory(const char* fileName);
	bool setDrawStatus(int status);
	bool deleteCurrentRect();
	bool cloneAndDrawRects();
	bool nextRect();
	bool prevRect();
	bool saveScreen();
	vector<IplImage*> extractROIRects(IplImage *image, vector<CaptureRect> rects);

// Play control extend
	virtual void draw();
};

#endif

