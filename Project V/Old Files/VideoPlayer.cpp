#include "VideoPlayer.h"
#include <stdio.h>
#include <string.h>

VideoPlayer* VideoPlayer::instance_ = NULL;
CvCapture* VideoPlayer::videoCapture_ = NULL;

VideoPlayer* VideoPlayer::setInstance(const char* video)
{
	if (instance_)
		delete instance_;
	instance_ = new VideoPlayer(video);

	return instance_;
}

VideoPlayer* VideoPlayer::getInstance()
{
	if (!instance_)
		throw "Instance is not set";

	return instance_;
}

VideoPlayer::VideoPlayer(const char* video)
{
	videoName_ = new char[strlen(video) + 1];
	strcpy(videoName_, video);
	playSpeed_ = 0;
	isPlaying_ = false;
	playStatus_ = PLAY;
	videoCapture_ = cvCreateFileCapture(videoName_);
	currFrame_ = cvQueryFrame(videoCapture_);
	window_ = (char*)"Video Player";
	trackbar_ = (char*)"Position";
	
	frames_ = (int)cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_FRAME_COUNT);
	fps_ = (int) cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_FPS);
	delayTime_ = getDelayTime();
	trackbarPos_ = 0;
}

int VideoPlayer::getDelayTime()
{
	return int(1000/fps_);
}

void VideoPlayer::handleTrackbarSlide(int pos)
{
	cvSetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES, pos);
}

void VideoPlayer::start()
{
	cvNamedWindow(window_, CV_WINDOW_AUTOSIZE);
	cvCreateTrackbar(trackbar_, window_, &trackbarPos_, frames_, onTrackBarSlide);

	while(1) {
		char c;
		if (playStatus_ == PLAY) {
			currFrame_ = cvQueryFrame(videoCapture_);
			cvSetTrackbarPos(trackbar_, window_, ++trackbarPos_);
		}
		
		if (!currFrame_) stop();
		cvShowImage(window_, currFrame_);

		c = cvWaitKey(delayTime_);
		if ((int)c == 27) break;
		if ((int)c != -1) handleKeyPressed(c);
	}
}

void VideoPlayer::handleKeyPressed(char c)
{
	switch (c) {
	case ' ':
		/*if ((playStatus_ == PAUSE) || (playStatus_ == STOP))*/
		if (playStatus_ == STOP || playStatus_ == PAUSE)
			playStatus_ = PLAY;
		else if (playStatus_ == PLAY)
			playStatus_ = PAUSE;
		break;
	case 'n': case 'N':
		nextFrame();
		break;
	case 'b': case 'B':
		previousFrame();
		break;
	case 's': case 'S':
		stop();
		break;
	case 'f': case 'F':
		doubleSpeed();
		break;
	case 'd': case 'D':
		halfSpeed();
		break;
	case 'g': case 'G':
		normalSpeed();
		break;
	default:
		break;
	}
	
}

void VideoPlayer::stop() {
	if (playStatus_ != STOP) {
		playStatus_ = STOP;
		cvSetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES, 0);
		cvSetTrackbarPos(trackbar_, window_, 0);
		currFrame_ = cvQueryFrame(videoCapture_);
		normalSpeed();
	}
}

VideoPlayer::~VideoPlayer(void)
{
	cvReleaseCapture(&videoCapture_);
	cvDestroyWindow(window_);
}

void VideoPlayer::nextFrame()
{
	if (playStatus_ != PLAY) {
		currFrame_ = cvQueryFrame(videoCapture_);
		cvSetTrackbarPos(trackbar_, window_, ++trackbarPos_);
	}
}

void VideoPlayer::previousFrame() {
	if (playStatus_ != PLAY) {
		int i = cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES);
		cvSetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES, i - 1);
		currFrame_ = cvQueryFrame(videoCapture_);
		cvSetTrackbarPos(trackbar_, window_, --trackbarPos_);
	}
}

void VideoPlayer::doubleSpeed() {
	if (playSpeed_ < maxSpeed_) {
		playSpeed_++;
		if (playSpeed_ != 0)
			delayTime_ = (int)delayTime_ / 2;
		else
			delayTime_ = getDelayTime();
	}
}

void VideoPlayer::halfSpeed() {
	if (playSpeed_ > -maxSpeed_) {
		playSpeed_--;
		if (playSpeed_ != 0)
			delayTime_ = delayTime_ * 2;
		else
			delayTime_ = getDelayTime();
	}
}

void VideoPlayer::normalSpeed() {
	playSpeed_ = 0;
	delayTime_ = getDelayTime();
}

void onTrackBarSlide(int pos) {
	VideoPlayer::getInstance()->handleTrackbarSlide(pos);
	//VideoPlayer::handleTrackbarSlide(pos);
}