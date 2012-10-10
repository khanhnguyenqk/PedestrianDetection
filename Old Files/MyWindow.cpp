#include "MyWindow.h"
#include <Fl/Fl.H>
#include <Fl/gl.h>
#include <Gl/GLU.h>

void IdleCallback(void* pData)
{
	if (pData != NULL)
	{
		MyWindow* pWindow = reinterpret_cast<MyWindow*>(pData);
		if (pWindow->animating)
		{
			pWindow->rotation += pWindow->rotationIncrement / 100;
			pWindow->redraw();
		}
	}
}

MyWindow::MyWindow(int h, int w, char* windowName): Fl_Gl_Window(w, h, windowName)
{
}


MyWindow::~MyWindow(void)
{
}

void MyWindow::initializeGl()
{
	glClearColor(.1f, .1f, .1f, 1);
	glEnable(GL_DEPTH_TEST);
}


void MyWindow::draw()
{
	static bool firstTime = true;
	if (firstTime)
	{
		initializeGl();
		firstTime = false;
	}// if

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // clear the color and depth buffer

	// view transformations
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);

	glRotatef(rotation, 0, 1, 0);

	// draw something
	DrawCube();
}


void MyWindow::DrawCube()
{
	glBegin(GL_QUADS);
	// front
	glColor3f(1, 0, 0);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, 1, 1);

	// back
	glColor3f(0, 1, 0);
	glVertex3f(-1, 1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(-1, -1, -1);

	// top
	glColor3f(0, 0, 1);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, -1);

	// bottom
	glColor3f(1, 1, 0);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	glVertex3f(-1, -1, 1);

	// left
	glColor3f(0, 1, 1);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, 1);

	// right
	glColor3f(1, 0, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glEnd();
}


int MyWindow::handle(int event)
{
	switch (event)
	{
	case FL_FOCUS:
	case FL_UNFOCUS:
		return 1;

	case FL_KEYBOARD:
		int key = Fl::event_key();
		switch (key)
		{
		case FL_Left:
			rotation -= rotationIncrement;
			redraw();
			return 1;

		case FL_Right:
			rotation += rotationIncrement;
			redraw();
			return 1;

		case ' ':
			animating = !animating;
			return 1;
		}
	}

	return Fl_Gl_Window::handle(event);
}

