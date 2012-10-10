#include "stdafx.h"
#include "VideoWindow.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <direct.h>
#include <sys/stat.h>


using namespace std;

double VideoWindow::playSpeed_ = 0;

bool VideoWindow::initOpenGl() {
	if(openGlInitialized_)
		return false;

	//initialize matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0,0,w(),h());
	openGlInitialized_ = true;

	return openGlInitialized_;
}


VideoWindow::~VideoWindow(void)
{
	cvReleaseImage(&currFrame_);
	cvReleaseCapture(&videoCapture_);
	delete videoName_;
}



bool VideoWindow::iplImage2Tex(const IplImage *image, GLuint &tex) {
	GLint format, type;

	/* Set pixel format */
	switch(image->nChannels)
	{
	case 1:
		format = GL_LUMINANCE;
		break;
	case 3:
		format = GL_BGR_EXT;
		break;
	case 4:
		format = GL_BGRA_EXT;
	default:
		return 0;
	}

	/* Set pixel type */
	switch(image->depth)
	{
	case IPL_DEPTH_8U:
		type = GL_UNSIGNED_BYTE;
		break;
	case IPL_DEPTH_8S:
		type = GL_BYTE;
		break;
	case IPL_DEPTH_16U:
		type = GL_UNSIGNED_SHORT;
		break;
	case IPL_DEPTH_16S:
		type = GL_SHORT;
		break;
	case IPL_DEPTH_32S:
		type = GL_INT;
		break;
	case IPL_DEPTH_32F:
		type = GL_FLOAT;
		break;
	case IPL_DEPTH_64F:
		/* Not supported by OpenGL */
	default:
		return 0;
	}

	/* Create texture object */
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	/* If necessary, specify texture parameters here */	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	/* Now upload pixels */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		image->width, image->height, 0,
		format, type, image->imageData);

	return true;
}

void VideoWindow::draw() {
	if (videoInitiated_) {
		if ((playStatus_ == PLAY)) {
			currFrame_ = cvQueryFrame(videoCapture_);

			drawBlackScreen();
			if (currFrame_ == NULL)
				stop();
			else {
				drawImageOnMainWindow(currFrame_);
			}
		} else if ((playStatus_ == PAUSE)) {
			drawBlackScreen();
			if (clone_ == NULL)
				stop();
			else {
				drawImageOnMainWindow(clone_);
			}
		} else if (playStatus_ == STOP)
			drawBlackScreen();
		/*printf("%f\n", playSpeed_);*/
	}
	updateDependences();
}

void VideoWindow::drawImageOnMainWindow(IplImage* image)
{
	double ox,oy;
	double x1,y1,x2,y2,x3,y3,x4,y4;
	ox=xPanRatio_*(1.0+zoomRatio_); oy=-yPanRatio_*(1.0+zoomRatio_);
	x1=ox-zoomRatio_; y1=oy-zoomRatio_;
	x2=ox+zoomRatio_; y2=oy-zoomRatio_;
	x3=ox+zoomRatio_; y3=oy+zoomRatio_;
	x4=ox-zoomRatio_; y4=oy+zoomRatio_;

	glEnable(GL_TEXTURE_2D);
	iplImage2Tex(image,textureName_);
	glBindTexture(GL_TEXTURE_2D, textureName_);
	glBegin( GL_QUADS );
	{
		glTexCoord2d(0.0,0.0); glVertex2d(x4, y4);
		glTexCoord2d(1.0,0.0); glVertex2d(x3, y3);
		glTexCoord2d(1.0,1.0); glVertex2d(x2, y2);
		glTexCoord2d(0.0,1.0); glVertex2d(x1, y1);
	}
	glEnd();
	glDeleteTextures(1, &textureName_);
}


void VideoWindow::drawBlackScreen() {
	glBegin( GL_QUADS );
	{
		glColor3d(0, 0, 0);
		glVertex2d(-1.0, -1.0);
		glVertex2d(1.0, -1.0);
		glVertex2d(1.0, 1.0);
		glVertex2d(-1.0, 1.0);
		glColor3d(1, 1, 1);
	}
	glEnd();
}

void VideoWindow::updateDependences() {
	if (videoCapture_) {
		int frameNum = (int)cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES);
		if (slider_ != NULL) {
			slider_->value(frameNum);
			slider_->maximum(frames_);
			/*slider_->redraw();*/
		}
		if (videoDetails_ != NULL) {
			char details[256];
			sprintf_s(details, "original fps: %4.2f | fps: %4.2f\ntotal frames: %010d | frame#: %010d", 
				videoFps_, playSpeed_, frames_, frameNum);
			videoDetails_->value(details);
		}
	} else {
		if (slider_ != NULL) {
			slider_->value(0);
			slider_->maximum(1);
		}
		if (videoDetails_ != NULL) {
			videoDetails_->value("No video selected");
		}
	}
}

// Play controls
bool VideoWindow::play() {
	if (playStatus_ != PLAY)
		playStatus_ = PLAY;
	else
		return false;
	return true;
}

bool VideoWindow::pause() {
	if (playStatus_ == PLAY) {
		if (clone_)
			cvReleaseImage(&clone_);
		clone_ = cvCloneImage(currFrame_);
		playStatus_ = PAUSE;
	}
	else
		return false;
	return true;
}

bool VideoWindow::showNextFrame() {
	if (!videoInitiated_)
		return false;
	if (playStatus_ != PLAY) {
		currFrame_ = cvQueryFrame(videoCapture_);
		if (clone_)
			cvReleaseImage(&clone_);
		clone_ = cvCloneImage(currFrame_);
	}
	else
		return false;
	return true;
}

bool VideoWindow::showPrevFrame() {
	if (!videoInitiated_)
		return false;
	if (playStatus_ != PLAY) {
		int i = (int)cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES);
		if (i > 1) {
			cvSetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES, i - 2);
			currFrame_ = cvQueryFrame(videoCapture_);
			if (clone_)
				cvReleaseImage(&clone_);
			clone_ = cvCloneImage(currFrame_);
		}
		else
			return false;

	} else
		return false;
	return true;
}

bool VideoWindow::jumpToFrame(int frameNum) {
	if (!videoInitiated_) return false;
	if (0 <= frameNum && frames_ >= frameNum) {
		cvSetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES, frameNum);
		currFrame_ = cvQueryFrame(videoCapture_);
		if (clone_)
			cvReleaseImage(&clone_);
		clone_ = cvCloneImage(currFrame_);
		return true;
	} else {
		return false;
	}
}

bool VideoWindow::stop() {
	if (playStatus_ != STOP) {
		playStatus_ = STOP;
		cvSetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES, 0);
	}
	else
		return false;
	return true;
}

bool VideoWindow::switchPlayPause() {
	if (playStatus_ == PLAY)
		pause();
	else if (playStatus_ != PLAY)
		play();
	else
		return false;
	return true;
}

bool VideoWindow::changePlaySpeed(double fps) {
	if (fps > videoFps_*maxSpeedRatio_ || fps < videoFps_/maxSpeedRatio_)
		return false;
	playSpeed_ = fps;
	return true;
}

bool VideoWindow::doubleSpeed() {
	return changePlaySpeed(playSpeed_ * 2);
}

bool VideoWindow::halfSpeed() {
	return changePlaySpeed(playSpeed_ / 2);
}

bool VideoWindow::setNormalSpeed() {
	return changePlaySpeed(videoFps_);
}


void VideoWindow::initiateVideo( const char* video )
{
	zoomRatio_=1.0;
	xPanRatio_=0.0;
	yPanRatio_=0.0;
	currFrame_ = NULL;

	// Setup video capture, get capture properties
	videoName_ = new char[strlen(video) + 1];
	strcpy_s(videoName_, strlen(video) + 1, video);
	videoCapture_ = cvCreateFileCapture(videoName_);
	if (!videoCapture_) {
		fl_alert("Can not open video file.");
		return;
	}

	// Play controls
	maxSpeedRatio_ = 4.0;
	playStatus_ = STOP;
	frames_ = (int)cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_FRAME_COUNT);
	playSpeed_ = videoFps_ = (int) cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_FPS);
	double frameWidth  = cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_FRAME_WIDTH);
	double frameHeight = cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_FRAME_HEIGHT);
	// Set window size to video's frame size
	/*this->size((int)frameWidth, (int)frameHeight);*/

	Fl::add_timeout(1.0/playSpeed_, timerCB, (void*)this);
	
	videoInitiated_ = true;
}

bool VideoWindow::isInitiated() {
	return videoInitiated_;
}