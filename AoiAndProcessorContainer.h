#pragma once
#include "AOITrapezium.h"
#include "MotionDetector.h"
#include <vector>

using namespace std;

class AoiAndProcessorContainer
{
protected:
	int count_;
	vector<AreaOfInterest*> aois_references_;
	vector<char*> window_names_;
	vector<MotionDetector*> detectors_;
public:
	AoiAndProcessorContainer(void);
	virtual ~AoiAndProcessorContainer(void);
	bool addAoi(AreaOfInterest* a);
};

