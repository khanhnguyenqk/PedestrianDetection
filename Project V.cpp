// Project V.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "stdafx.h"
#include "Project V.h"

#include "VideoPlayerUI.h"
#include <Eigen/Dense>
#include "LinearAlgebra.h"
#include "ForegroundObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace Eigen;

int main(int argc, char** argv) {
  MatrixXd m1(3,1);
  MatrixXd m2(3,1);
  m1 << -1,
    -1,
    1;
  m2 << 1,
    -1,
    -1;
  CvPoint2D32f p = findIntersection(m1, m2);

	Fl::visual(FL_RGB);
	VideoPlayerUI* ui = new VideoPlayerUI;
	ui->mainWindow_->show(1, NULL);
	ui->setFilePath("C:\\Materials\\2d.avi");

	Fl::run();

	return 0;
}