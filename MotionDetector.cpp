#include "StdAfx.h"
#include "MotionDetector.h"


MotionDetector::MotionDetector(CvSize imgSize)
{
	imgSize_ = imgSize;
	colourImage_ = NULL;
	difference_ = NULL;
	temp_ = NULL;
	greyImage_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 1);
	movingAverage_ = cvCreateImage(imgSize_, IPL_DEPTH_32F, 3);
	motionHistory_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 3);
	first_ = true;

	// Parameters
	alpha_ = 0.020;
}


MotionDetector::~MotionDetector(void)
{
	cvReleaseImage(&temp_);
	cvReleaseImage(&difference_);
	cvReleaseImage(&greyImage_);
	cvReleaseImage(&movingAverage_);
	cvReleaseImage(&colourImage_);
	cvReleaseImage(&motionHistory_);
}

IplImage* MotionDetector::processPicture(IplImage* frame) {
	if (colourImage_ != NULL)
		cvReleaseImage(&colourImage_);
	colourImage_ = cvCloneImage(frame);
	if(first_)
	{
		difference_ = cvCloneImage(colourImage_);
		temp_ = cvCloneImage(colourImage_);
		cvConvertScale(colourImage_, movingAverage_, 1.0, 0.0);
		first_ = false;
	} else {
		cvRunningAvg(colourImage_, movingAverage_, alpha_, NULL);
	}

	//Convert the scale of the moving average.
	cvConvertScale(movingAverage_,temp_, 1.0, 0.0);

	//Minus the current frame from the moving average.
	cvAbsDiff(colourImage_,temp_,difference_);

	//Convert the image to gray scale.
	cvCvtColor(difference_,greyImage_,CV_RGB2GRAY);

	//Convert the image to black and white.
	cvThreshold(greyImage_, greyImage_, 70, 255, CV_THRESH_BINARY);

	//Dilate and erode to get people blobs
	cvDilate(greyImage_, greyImage_, 0, 18);
	cvErode(greyImage_, greyImage_, 0, 10);

	//Find the contours of the moving images in the frame.
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	cvFindContours( greyImage_, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	//Process each moving contour in the current frame...
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

		cvRectangle(colourImage_, pt1, pt2, CV_RGB(255,0,0), 1);

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
		//	cvRectangle(colourImage_, pt1, pt2, CV_RGB(255,0,0), 1);
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
	//cvPutText(colourImage_, _itoa(numObj_, wow, 10), cvPoint(10, 10), &font, cvScalar(0, 0, 300));

	cvReleaseMemStorage(&storage);

	////Write the frame to the output movie.
	//cvWriteFrame(outputMovie, colourImage_);
	IplImage *ret = cvCloneImage(colourImage_);
	return ret;
}