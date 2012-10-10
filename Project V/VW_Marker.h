#ifndef VW_MARKER_H_
#define VW_MARKER_H_

#include <vector>
#include "videowindow.h"
#include "CaptureRect.h"
#include "ColorChooser.h"

using namespace std;

typedef vector<CaptureRect>::iterator CR_Iterator;

class VW_Marker :
	public VideoWindow
{
private:
	bool cloneDone_;
	CaptureRect cr_;
	vector<CaptureRect> captureRects_;
	CR_Iterator currentRect_;
	int drawStatus_;
	bool drawnOrChanged_;
	double filePercentage_;
	CvPoint lastMousePoint_;
	ColorChooser colorChooser_;
public:
	VW_Marker(int x,int y , int w, int h,
		const char* video=0, const char *L=0):VideoWindow(x,y,w,h,video,L) {
		cloneDone_ = true;
		drawStatus_ = NEW_RECT;
		filePercentage_ = 0.95;
	}
	virtual ~VW_Marker(void);
	virtual int handle(int event);

// Draw rect
private:
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
	IplImage** extractROIRects(IplImage *image, vector<CaptureRect> rects, int &size);

// Play control extend
	virtual void draw();
	/*virtual bool play() {
	captureRects_.clear();
	drawnOrChanged_ = false; 
	return VideoWindow::play();}
	virtual bool showNextFrame() {
	captureRects_.clear();
	drawnOrChanged_ = false; 
	return VideoWindow::showNextFrame();}
	virtual bool showPrevFrame() {
	captureRects_.clear();
	drawnOrChanged_ = false; 
	return VideoWindow::showPrevFrame();}
	virtual bool jumpToFrame(int frameNum) {
	captureRects_.clear();
	drawnOrChanged_ = false; 
	return VideoWindow::jumpToFrame(frameNum);}*/
};

#endif

