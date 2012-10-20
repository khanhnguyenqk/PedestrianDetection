#ifndef CAPTURE_RECT_H_
#define CAPTURE_RECT_H_

#include <opencv2/opencv.hpp>

enum DrawAction {NEW_RECT, MOVE_RECT, RESIZE_TL, RESIZE_TR, RESIZE_BR, RESIZE_BL};

class CaptureRect
{
protected:
	// Area percentage of the corner compared to the total area
	double cornerPercentage_;
	CvRect rect_;
	CvScalar color_;
	CvScalar* colorOrg_;
	int thickness_;
public:
	CaptureRect(void);
	~CaptureRect(void);
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
	void resize(int w, int h);
	void resizeAddVector(CvPoint vector);
	void move(CvPoint vector);
	void darkenColor();
	void returnOriginalColor();
	void fixNegativeWH();
	bool contains(CvPoint p);
	void moveCorner(int drawMethod, CvPoint vector);

	virtual int actionController(CvPoint mousePointer);
};


#endif
