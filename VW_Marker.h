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
#include "DrawToolLine.h"

using namespace std;

class VideoWindowMarker :
	public VideoWindow
{
protected:
	bool useRect_;
  bool newTool_;
	bool cloneDone_;
  int new0_modify1_;
  int aoi0_line1_;
  int modifyCodeNum_;

	vector<AreaOfInterest*> aois_;
	vector<AreaOfInterest*>::iterator currentAoi_;
  vector<DrawToolLine*> lines_;
  vector<DrawToolLine*>::iterator currentLine_;
	
	bool drawnOrChanged_;
	CvPoint lastMousePoint_;
	ColorChooser colorChooser_;
public:
	VideoWindowMarker(int x,int y , int w, int h,
		const char* video=0, const char *L=0):VideoWindow(x,y,w,h,video,L) {
		useRect_ = true;
		cloneDone_ = true;
    newTool_ = true;
    aoi0_line1_ = 1;
    new0_modify1_ = -1;
	}
	virtual ~VideoWindowMarker(void);
	virtual int handle(int event);

// Draw rect
protected:
	void drawAllTools(IplImage* img);
	int mouseCreateToolHandle(int event);
	int mouseModifyToolHandle(int event);
  void chooseDrawAction(CvPoint mouse);
  void markNewToolFlag();
	int getRelativeMouseX(int x);
	int getRelativeMouseY(int y);

// Other
public:
	string getSaveDirectory(const char* fileName);
	bool setDrawStatus(int status = 0);
	bool deleteCurrentTool();
	bool deleteAllTools();
	bool cloneAndDrawTools();
	bool nextAOI();
	bool prevAOI();
	void useRectangle(bool b);
	bool isUsingRectangle();
  void setDrawLineFlag();

// Play control extend
	virtual void draw();
};

#endif