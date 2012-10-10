#ifndef COLORCHOOSER_H_
#define COLORCHOOSER_H_

// This hold 16 Tertiary colors and return a single color for caller
//	Colors are kept using CvScalar
#include <list>
#include <opencv2/opencv.hpp>

using namespace std;

class ColorChooser
{
private:
	list<CvScalar> rainbow_;
	list<CvScalar>::iterator current_;
public:
	CvScalar getAColor();
	ColorChooser(void);
	virtual ~ColorChooser(void);
};

#endif