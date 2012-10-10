#ifndef VIDEO_WINDOW_H_
#define VIDEO_WINDOW_H_

#include <string>

#include <Fl/Fl.H>
#include <Fl/Fl_Gl_Window.H>
#include <Fl/gl.h>
#include <fl/glu.h>
#include <fl/fl_ask.H>
#include <fl/Fl_Native_File_Chooser.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Slider.H>

#include <opencv2/opencv.hpp>

#define LAG_FIX 0.001

using namespace std;

enum {PLAY, PAUSE, STOP};

class VideoWindow : public Fl_Gl_Window
{
protected:
	bool videoInitiated_;
	CvCapture* videoCapture_;
	IplImage* currFrame_;
	IplImage* clone_;
	char* videoName_;
	GLuint textureName_;
	
	bool openGlInitialized_;
	
	// Play controls
	double maxSpeedRatio_;
	int playStatus_;
	int frames_;
	double videoFps_;
	static double playSpeed_;
	
	// Dependences
	Fl_Slider *slider_;
	Fl_Multiline_Output *videoDetails_;
public:
	double zoomRatio_;  //0.1  10.0
	double xPanRatio_,yPanRatio_; // -1.0	1.0
protected:
	bool initOpenGl();
	bool iplImage2Tex(const IplImage *image, GLuint &tex);
	void drawBlackScreen();
	void updateDependences();
	void freeVideoMem();
public:
	VideoWindow::VideoWindow(int x,int y , int w, int h,
						const char* video=0, const char *L=0):Fl_Gl_Window(x,y,w,h,L)
	{
		openGlInitialized_ = false;
		if(!initOpenGl())
		{
			fl_alert("Can not init OpenGL. Exit...");
			exit(0);
		}

		clone_ = NULL;
		videoInitiated_ = false;
		maxSpeedRatio_ = 4.0;
		playStatus_ = STOP;
		videoCapture_ = NULL;
		currFrame_ = NULL;
		videoName_ = NULL;
		slider_ = NULL;
		videoDetails_ = NULL;
		if (video)
			initiateVideo(video);
		Fl::add_timeout(1.0/playSpeed_, timerCB, (void*)this);
	}
	virtual ~VideoWindow(void);
	static void timerCB(void *userData) {
		VideoWindow* w = (VideoWindow*)userData;
		w->redraw();
		Fl::repeat_timeout(1.0/playSpeed_ + LAG_FIX, timerCB, userData);
	}
	void initiateVideo( const char* video );
	virtual int handle(int event) {return Fl_Gl_Window::handle(event);}
public:
	virtual void draw();
	void drawImageOnMainWindow(IplImage* image);
	bool isInitiated();

	virtual void addDependent(Fl_Slider* slider) {slider_ = slider;}
	virtual void addDependent(Fl_Multiline_Output* videoDetails) {videoDetails_ = videoDetails;}

	// Play controls
public:
	virtual bool play();
	virtual bool stop();
	virtual bool pause();
	virtual bool switchPlayPause();
	virtual bool changePlaySpeed(double fps);
	virtual bool setNormalSpeed();
	virtual bool doubleSpeed();
	virtual bool halfSpeed();
	virtual bool showNextFrame();
	virtual bool showPrevFrame();
	virtual bool jumpToFrame(int frameNum);
};

#endif