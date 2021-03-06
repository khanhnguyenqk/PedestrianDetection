#pragma once
#include "AreaOfInterest.h"

enum DrawActionExtension {
	TRAPEZIUM_P0 = 6,
	TRAPEZIUM_P1, TRAPEZIUM_P2, TRAPEZIUM_P3
};

class AoiTrapezium :
	public AreaOfInterest
{
protected:
	double cornerLengthPercentage_;
	CvPoint2D32f pts_[4];
public:
	AoiTrapezium(void);
	virtual ~AoiTrapezium(void);
protected:
	void fixRectangleBoundary();
	double maxFour(double a, double b, double c, double d);
	double minFour(double a, double b, double c, double d);
public:
	// Accessors
	CvPoint2D32f getPoint(int it);
	// When user finish drawing the rectangle, assign 4 points to 4 corners of the
	//	rectangle.
	void reassignedCorners();
	virtual void move(CvPoint vector);
	virtual int actionController(CvPoint mousePointer, bool callParent = true);
	virtual void modify(int drawMethod, CvPoint vector);
	static bool isConvex(CvPoint2D32f pts[4]);
	virtual void drawSelfOnImage(IplImage* img);
  virtual bool doesContainPoint(CvPoint p);
};