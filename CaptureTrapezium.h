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
	CvPoint2D32f p1_, p2_, p3_, p4_;
public:
	CaptureTrapezium(void);
	virtual ~CaptureTrapezium(void);
protected:
public:
	// When user finish drawing the rectangle, assign 4 points to 4 corners of the
	//	rectangle.
	void finishCreating();
	virtual int actionController(CvPoint mousePointer);
};