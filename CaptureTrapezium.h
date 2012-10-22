#pragma once
#include "capturerect.h"

enum DrawAction_Ext {
	P1 = 6,
	P2, P3, P4
};

class CaptureTrapezium :
	public CaptureRect
{
protected:
	double cornerLengthPercentage_;
	CvPoint2D32f p1_, p2_, p3_, p4_;
public:
	CaptureTrapezium(void);
	virtual ~CaptureTrapezium(void);
protected:
	void fixBoundaryRect();
	double maxFour(double a, double b, double c, double d);
	double minFour(double a, double b, double c, double d);
public:
	// Accessor
	CvPoint2D32f getP1();
	CvPoint2D32f getP2();
	CvPoint2D32f getP3();
	CvPoint2D32f getP4();
	// When user finish drawing the rectangle, assign 4 points to 4 corners of the
	//	rectangle.
	void finishCreating();
	virtual void move(CvPoint vector);
	virtual int actionController(CvPoint mousePointer);
	virtual void moveCorner(int drawMethod, CvPoint vector);
};