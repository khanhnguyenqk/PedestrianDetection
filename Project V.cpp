// Project V.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "stdafx.h"
#include "Project V.h"

#include "VideoPlayerUI.h"
#include <Eigen/Dense>
#include "LinearAlgebra.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace Eigen;

int main(int argc, char** argv) {
	Fl::visual(FL_RGB);
	VideoPlayerUI* ui = new VideoPlayerUI;
	ui->mainWindow_->show(1, NULL);
	ui->setFilePath("C:\\Materials\\1.avi");

	Fl::run();

	/*int aX, aY, bX, bY;
	cin >> aX >> aY >> bX >> bY;
	try {
	LineSegment2D ret = findLineSegmentFormular2D(cvPoint2D32f(aX, aY), cvPoint2D32f(bX, bY));
	cout << ret.line2D_ << endl;
	double x = findX(ret, 0);
	cout << x << endl;
	} catch (...) {
	cout << "Fucked in the ass.\n";
	}

	cin >> aX;*/

	return 0;
}