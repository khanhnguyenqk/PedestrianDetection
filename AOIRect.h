#ifndef CAPTURE_RECT_H_
#define CAPTURE_RECT_H_

#include <opencv2/opencv.hpp>

enum DrawAction {NEW_RECT, MOVE_RECT, RESIZE_TL, RESIZE_TR, RESIZE_BR, RESIZE_BL};

class AOIRect
{
protected:
	// Area percentage of the corner compared to the total area
	double cornerAreaPercentage_;
	CvRect rect_;
	CvScalar color_;
	CvScalar* colorOrg_;
	int thickness_;
public:
	AOIRect(void);
	~AOIRect(void);
protected:
	
public:
	//Accessors
	void setRect(int x, int y, int w, int h);
	void setRect(CvRect rect);
	void setColor(double r, double g, double b);
	void setColor(CvScalar color);
	CvRect getRect();
	CvScalar getColor();
	CvPoint getBottomRightCorner();
	int getThickness();

	// Mutators
	void darkenColor();
	void returnOriginalColor();

	void resize(int w, int h);
	void resizeAddVector(CvPoint vector);
	virtual void move(CvPoint vector);
	void fixNegativeWH();
	bool contains(CvPoint p);
	virtual void moveCorner(int drawMethod, CvPoint vector);

	virtual int actionController(CvPoint mousePointer);
	virtual void drawSelfOnImage(IplImage* img);

	virtual void reassignedCorners() = 0;
	virtual CvPoint2D32f getPoint(int i) = 0;
};


#endif
