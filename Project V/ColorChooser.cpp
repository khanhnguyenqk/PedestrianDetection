#include "stdafx.h"
#include "ColorChooser.h"


ColorChooser::ColorChooser(void)
{
	rainbow_.push_back(cvScalar(255,0,0)); rainbow_.push_back(cvScalar(255,128,0));
	rainbow_.push_back(cvScalar(255,255,0)); rainbow_.push_back(cvScalar(128,255,0));
	rainbow_.push_back(cvScalar(255,0,128)); rainbow_.push_back(cvScalar(255,61,61));
	rainbow_.push_back(cvScalar(255,122,122)); rainbow_.push_back(cvScalar(0,255,0));
	rainbow_.push_back(cvScalar(255,0,255)); rainbow_.push_back(cvScalar(122,255,255));
	rainbow_.push_back(cvScalar(61,255,255)); rainbow_.push_back(cvScalar(0,255,128));
	rainbow_.push_back(cvScalar(128,0,255)); rainbow_.push_back(cvScalar(0,0,255));
	rainbow_.push_back(cvScalar(0,128,255)); rainbow_.push_back(cvScalar(0,255,255));
	

	current_ = rainbow_.begin();
}

CvScalar ColorChooser::getAColor() {
	if (rainbow_.size() == 0)
		throw "Something wrong here. Write some meaningful error or fix it";
	CvScalar ret = *current_;
	current_++;
	if (current_ == rainbow_.end())
		current_ = rainbow_.begin();
	return ret;
}

ColorChooser::~ColorChooser(void)
{
}
