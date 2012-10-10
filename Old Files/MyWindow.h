#pragma once
#include <Fl/Fl_Gl_Window.H>

class MyWindow :
	public Fl_Gl_Window
{
public:
	MyWindow(int w, int h, char* windowName);
	virtual ~MyWindow(void);
	void initializeGl();
	virtual void draw();
	void DrawCube();
	float rotation, rotationIncrement;
	bool animating;
	virtual int handle(int event);
};