#ifndef VIDEOPLAYER_H_
#define VIDEOPLAYER_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

enum {PLAY, PAUSE, STOP};

// Singleton class, can't find a way to make this multiple
//	instances class yet due to the handle action of track bar, etc.
class VideoPlayer
{
	static VideoPlayer* instance_;
	VideoPlayer(const char* video);
	~VideoPlayer(void);

	static const int maxSpeed_ = 4;
	int playSpeed_;
	bool isPlaying_;
	int playStatus_;
	char* videoName_;
	static CvCapture* videoCapture_;
	IplImage* currFrame_;
	char* window_;
	char* trackbar_;
	char** switches_;
	int frames_;
	int fps_;
	int delayTime_;
	int trackbarPos_;

	int getDelayTime();
	void handleKeyPressed(char c);
	void nextFrame();
	void previousFrame();
	void stop();
	void doubleSpeed();
	void halfSpeed();
	void normalSpeed();
public:
	/*VideoPlayer(const char* video);
	~VideoPlayer(void);*/
	static VideoPlayer* setInstance(const char* video);
	static VideoPlayer* getInstance();
	static void handleTrackbarSlide(int pos);
	void start();
};

void onTrackBarSlide(int pos);

#endif