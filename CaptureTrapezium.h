#pragma once
#include "capturerect.h"

enum DrawAction_Ext {
	P0 = 6,
	P1, P2, P3
};

class CaptureTrapezium :
	public CaptureRect
{
protected:
	double cornerLengthPercentage_;
	CvPoint2D32f pts_[4];
public:
	CaptureTrapezium(void);
	virtual ~CaptureTrapezium(void);
protected:
	void fixBoundaryRect();
	double maxFour(double a, double b, double c, double d);
	double minFour(double a, double b, double c, double d);
public:
	// Accessor
	CvPoint2D32f getPoint(int it);
	// When user finish drawing the rectangle, assign 4 points to 4 corners of the
	//	rectangle.
	void finishCreating();
	virtual void move(CvPoint vector);
	virtual int actionController(CvPoint mousePointer);
	virtual void moveCorner(int drawMethod, CvPoint vector);
	static bool isConvex(CvPoint2D32f pts[4]);
};