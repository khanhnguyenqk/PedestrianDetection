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
	ui->setFilePath("C:\\Materials\\6r.avi");

	Fl::run();

	/*RowVector3d a, b, c;
	a << 3, 3, 0;
	b << 4, 1, 0;
	c << 3.5, 4, 0;
	cout << a.cross(b) / a.dot(b) << endl;
	cout << c.cross(b) / c.dot(b) << endl;
	cout << (a.cross(b) / a.dot(b)).dot(c.cross(b) / c.dot(b)) << endl;
	int i;
	cin >> i;*/

	return 0;
}