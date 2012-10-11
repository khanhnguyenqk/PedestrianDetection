// Project V.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "stdafx.h"
#include "Project V.h"

#include "VideoPlayerUI.h"
#include "Boundary.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

int main(int argc, char** argv) {
	Fl::visual(FL_RGB);
	VideoPlayerUI* ui = new VideoPlayerUI;
	ui->mainWindow_->show(1, NULL);
	ui->setFilePath("C:\\Materials\\1.avi");

	Fl::run();

	return 0;
}