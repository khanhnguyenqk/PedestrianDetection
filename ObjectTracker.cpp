#include "StdAfx.h"
#include "ObjectTracker.h"

ObjectTracker::ObjectTracker(CvSize imgSize)
{
  openIteration_ = 1;
  closeIteration_ = 2;
  first_ = true;
  mog_ = new ExLBMixtureOfGaussians;
}


ObjectTracker::~ObjectTracker(void)
{
}

void ObjectTracker::processImage( IplImage* frame, IplImage** output )
{
  imgSize_ = cvSize(frame->width, frame->height);
  if (first_) {
    temp_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 3);
    mask3C_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 3);
    mask_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 1);
    first_ = false;
  }

  if (*output == NULL) {
    *output = cvCreateImage(imgSize_, IPL_DEPTH_8U, 3);
  }
  cvCopy(frame, *output);
  cvCopy(frame, temp_);
  cvSmooth(temp_, temp_, CV_GAUSSIAN);
  mog_->process((cv::Mat)temp_, (cv::Mat)mask3C_);
  cvCvtColor(mask3C_, mask_, CV_RGB2GRAY);

  cvShowImage("1", mask_);
  cvMorphologyEx(mask_, mask_, 0, 0, CV_MOP_OPEN, openIteration_);
  cvShowImage("2", mask_);
  cvMorphologyEx(mask_, mask_, 0, 0, CV_MOP_CLOSE, closeIteration_);
  cvShowImage("3", mask_);

  CvPoint centers[50];
  int count = 50;
  findConnectedComponents(mask_, 0, 0.05, &count, NULL, centers);
  cvCvtColor(mask_, mask3C_, CV_GRAY2RGB);
  for (int i=0; i<count; i++) {
    cvCircle(mask3C_, centers[i], 2, cvScalar(255, 0, 0));
  }
  cvShowImage("4", mask3C_);
}

void ObjectTracker::findConnectedComponents( IplImage* mask, int poly1_hull2 /* = 0 */, float perimScale /* = 0.25 */, int* num /* = NULL */, CvRect* bbs /* = NULL */, CvPoint* centers /* = NULL */ ) {
  int cvContourApproxLevel = 2;

  static CvMemStorage *mem_storage = NULL;
  static CvSeq *contours = NULL;

  if (mem_storage == NULL) {
    mem_storage = cvCreateMemStorage(0);
  } else {
    cvClearMemStorage(mem_storage);
  }

  CvContourScanner scanner = cvStartFindContours(mask, mem_storage, sizeof(CvContour),
    CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  CvSeq *c;
  int numCont = 0;
  while ((c = cvFindNextContour(scanner)) != NULL) {
    double len = cvContourPerimeter(c);

    double q = (mask->height + mask->width) * perimScale;

    if (len < q) {
      cvSubstituteContour(scanner, NULL);
    } else {
      if (poly1_hull2) {
        CvSeq *c_new;
        if (poly1_hull2 == 1) {
          c_new = cvApproxPoly(c, sizeof(CvContour), mem_storage, CV_POLY_APPROX_DP,
            cvContourApproxLevel, 0);
        } else if (poly1_hull2 == 2){
          c_new = cvConvexHull2(c, mem_storage, CV_CLOCKWISE, 1);
        }
        cvSubstituteContour(scanner, c_new);
      }
      numCont++;
    }
  }

  contours = cvEndFindContours(&scanner);

  const CvScalar CVX_WHITE = CV_RGB(0xff,0xff,0xff);
  const CvScalar CVX_BLACK = CV_RGB(0x00,0x00,0x00);

  cvZero(mask);
  IplImage *maskTemp;

  // CALC CENTER OF MASS AND/OR BOUNDING RECTANGLES
  //
  if(num != NULL) {
    //User wants to collect statistics
    //
    int N = *num, numFilled = 0, i=0;
    CvMoments moments;
    double M00, M01, M10;
    maskTemp = cvCloneImage(mask);
    for(i=0, c=contours; c != NULL; c = c->h_next,i++ ) {
      if(i < N) {
        // Only process up to *num of them
        //
        cvDrawContours(
          maskTemp,
          c,
          CVX_WHITE,
          CVX_WHITE,
          -1,
          CV_FILLED,
          8
          );
        // Find the center of each contour
        //
        if(centers != NULL) {
          cvMoments(maskTemp,&moments,1);
          M00 = cvGetSpatialMoment(&moments,0,0);
          M10 = cvGetSpatialMoment(&moments,1,0);
          M01 = cvGetSpatialMoment(&moments,0,1);
          centers[i].x = (int)(M10/M00);
          centers[i].y = (int)(M01/M00);
        }
        //Bounding rectangles around blobs
        //
        if(bbs != NULL) {
          bbs[i] = cvBoundingRect(c);
        }
        cvZero(maskTemp);
        numFilled++;
      }
      // Draw filled contours into mask
      //
      cvDrawContours(
        mask,
        c,
        CVX_WHITE,
        CVX_WHITE,
        -1,
        CV_FILLED,
        8
        );
    } //end looping over contours
    *num = numFilled;
    cvReleaseImage( &maskTemp);
  }
  // ELSE JUST DRAW PROCESSED CONTOURS INTO THE MASK
  //
  else {
    // The user doesn't want statistics, just draw the contours
    //
    for( c=contours; c != NULL; c = c->h_next ) {
      cvDrawContours(
        mask,
        c,
        CVX_WHITE,
        CVX_BLACK,
        -1,
        CV_FILLED,
        8
        );
    }
  }
}