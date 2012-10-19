#include "StdAfx.h"
#include "MotionDetector.h"


MotionDetector::MotionDetector(CvSize imgSize)
{
	methodCode_ = SIMPLE;
	first_ = true;
	//
	// Simple method
	// 
	imgSize_ = imgSize;
	sDifference_ = NULL;
	sTemp_ = NULL;
	sGreyImage_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 1);
	sMovingAverage_ = cvCreateImage(imgSize_, IPL_DEPTH_32F, 3);
	
	// Parameters
	alpha_ = 0.020;
	thresvalue_ = 50;
	dilateIterations_ = 18;
	erodeIterations_ = 10;

	//
	// Background average
	// 
	count_ = 0.00001;	// Protect against divide by zero
	upperScale_ = 7.0; lowerScale_ = 6.0;

	mean_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 3 );
	deviation_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 3 );
	prev_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 3 );
	hi_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 3 );
	low_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 3 );
	temp1_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 3 );
	temp2_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 3 );

	low1_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );
	low2_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );
	low3_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );
	hi1_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );
	hi2_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );
	hi3_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );
	gray1_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );
	gray2_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );
	gray3_ = cvCreateImage( imgSize, IPL_DEPTH_32F, 1 );

	mask1_ = cvCreateImage( imgSize, IPL_DEPTH_8U, 1 );
	mask2_ = cvCreateImage( imgSize, IPL_DEPTH_8U, 1 );
	
	cvZero( mean_ );
	cvZero( deviation_ );
	cvZero( prev_ );
	cvZero( hi_ );
	cvZero( low_ );
	cvZero( temp1_ );
	cvZero( temp2_ );
}


MotionDetector::~MotionDetector(void)
{
	cvReleaseImage(&sTemp_);
	cvReleaseImage(&sDifference_);
	cvReleaseImage(&sGreyImage_);
	cvReleaseImage(&sMovingAverage_);
}

IplImage* MotionDetector::processImage(IplImage* frame) {
	if (methodCode_ == SIMPLE) {
		return sProcessImage(frame);
	}
}

IplImage* MotionDetector::sBackGroudDiff(IplImage *frame) {
	/*if (sColorImage_ != NULL)
	cvReleaseImage(&sColorImage_);
	sColorImage_ = cvCloneImage(frame);*/

	if(first_)
	{
		sDifference_ = cvCloneImage(frame);
		sTemp_ = cvCloneImage(frame);
		cvConvertScale(frame, sMovingAverage_, 1.0, 0.0);
		first_ = false;
	} else {
		cvRunningAvg(frame, sMovingAverage_, alpha_, NULL);
	}

	cvConvertScale(sMovingAverage_,sTemp_, 1.0, 0.0);
	cvAbsDiff(frame,sTemp_,sDifference_);
	cvCvtColor(sDifference_,sGreyImage_,CV_RGB2GRAY);

	cvThreshold(sGreyImage_, sGreyImage_, thresvalue_, 255, CV_THRESH_BINARY);
	return sGreyImage_;
}

IplImage* MotionDetector::sProcessImage(IplImage *frame) {
	IplImage* diff = sBackGroudDiff(frame);
	//cvAdaptiveThreshold(sGreyImage_, sGreyImage_, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV);
	//Dilate and erode to get object blobs
	cvDilate(sGreyImage_, sGreyImage_, 0, dilateIterations_);
	cvErode(sGreyImage_, sGreyImage_, 0, erodeIterations_);

	//Find the contours of the moving images in the frame.
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	cvFindContours( sGreyImage_, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	for( ; contour != 0; contour = contour->h_next )
	{
		CvRect bndRect;
		CvPoint pt1, pt2;

		//Get a bounding rectangle around the moving object.
		bndRect = cvBoundingRect(contour, 0);

		pt1.x = bndRect.x;
		pt1.y = bndRect.y;
		pt2.x = bndRect.x + bndRect.width;
		pt2.y = bndRect.y + bndRect.height;

		cvRectangle(frame, pt1, pt2, CV_RGB(255,0,0), 1);

		////Get an average X position of the moving contour.
		//avgX = (pt1.x + pt2.x) / 2;

		////If the contour is within the edges of the building...
		//if(avgX > 90 && avgX < 250)
		//{
		//	//If the the previous contour was within 2 of the left boundary...
		//	if(closestToLeft >= 88 && closestToLeft <= 90)
		//	{
		//		//If the current X position is greater than the previous...
		//		if(avgX > prevX)
		//		{
		//			//Increase the number of people.
		//			numPeople++;

		//			//Reset the closest object to the left indicator.
		//			closestToLeft = 0;
		//		}
		//	}
		//	//else if the previous contour was within 2 of the right boundary...
		//	else if(closestToRight >= 250 && closestToRight <= 252)
		//	{
		//		//If the current X position is less than the previous...
		//		if(avgX < prevX)
		//		{
		//			//Increase the number of people.
		//			numPeople++;

		//			//Reset the closest object to the right counter.
		//			closestToRight = 320;
		//		}
		//	}

		//	//Draw the bounding rectangle around the moving object.
		//	cvRectangle(sColorImage_, pt1, pt2, CV_RGB(255,0,0), 1);
		//}

		////If the current object is closer to the left boundary but still not across
		////it, then change the closest to the left counter to this value.
		//if(avgX > closestToLeft && avgX <= 90)
		//{
		//	closestToLeft = avgX;
		//}

		////If the current object is closer to the right boundary but still not across
		////it, then change the closest to the right counter to this value.
		//if(avgX < closestToRight && avgX >= 250)
		//{
		//	closestToRight = avgX;
		//}

		////Save the current X value to use as the previous in the next iteration.
		//prevX = avgX;
	}

	//CvFont font;
	//char wow[64];
	////Write the number of people counted at the top of the output frame.		
	//cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.8, 0.8, 0, 2);
	//cvPutText(sColorImage_, _itoa(numObj_, wow, 10), cvPoint(10, 10), &font, cvScalar(0, 0, 300));

	cvReleaseMemStorage(&storage);

	////Write the frame to the output movie.
	//cvWriteFrame(outputMovie, sColorImage_);
	IplImage *ret = cvCloneImage(frame);
	return ret;
}

CvSeq* MotionDetector::findContours(IplImage *singleChannelPic) { 
	return NULL;
}

void MotionDetector::accumulateBackground(IplImage *frame) {
	cvCvtScale(frame, temp1_, 1, 0);
	if (first_) {
		first_ = false;
	} else {
		cvAcc(temp1_, total_);
		cvAbsDiff(temp1_, prev_, temp2_);
		cvAcc(temp2_, totalDiff_);
		count_ += 1;
	}
	cvCopy(temp1_, prev_);
}

void MotionDetector::createModelsFromStats() {
	cvConvertScale(total_, mean_, 1.0/count_);
	cvConvertScale(totalDiff_, deviation_, 1.0/count_);

	//Make sure deviation is always something
	//
	cvAddS(deviation_, cvScalar(1, 1, 1), deviation_);
	setThreshold();
}

void MotionDetector::setThreshold() {
	// Upper threshold
	cvConvertScale(deviation_, temp1_, upperScale_);
	cvAdd(temp1_, mean_, hi_);
	cvSplit(hi_, hi1_, hi2_, hi3_, NULL);

	// Lower threshold
	cvConvertScale(deviation_, temp1_, lowerScale_);
	cvSub(mean_, temp1_, low_);
	cvSplit(low_, low1_, low2_, low3_, NULL);
}

void MotionDetector::backGroundDiff(IplImage *frame) {
	cvCvtScale(frame, temp1_, 1, 0);
	cvSplit(temp1_, gray1_, gray2_, gray3_, NULL);

	cvInRange(gray1_, low1_, hi1_, mask1_);

	cvInRange(gray2_, low2_, hi2_, mask2_);
	cvOr(mask1_, mask2_, mask1_);

	cvInRange(gray3_, low3_, hi3_, mask2_);
	cvOr(mask1_, mask2_, mask1_);

	cvSubRS(mask1_, cvScalar(255), mask1_);
}