#include "StdAfx.h"
#include "ObjectTracker.h"

ObjectTracker::ObjectTracker(CvSize imgSize)
{
  openIteration_ = 1;
  closeIteration_ = 2;
  first_ = true;
  mog_ = new ExLBMixtureOfGaussians;
  numStat_ = 100;
  numObjs_ = 0;
  perimScaleThrhold_ = 0.05;
  matchThreshold_ = 0.11;
}


ObjectTracker::~ObjectTracker(void)
{
}

void drawCross(IplImage *img, CvPoint center, int size, CvScalar color, int thickness = 1) {
  cvLine(img, cvPoint(center.x+size, center.y+size),
    cvPoint(center.x-size, center.y-size), color, 1);
  cvLine(img, cvPoint(center.x+size, center.y-size),
    cvPoint(center.x-size, center.y+size), color, 1);
}

void ObjectTracker::processImage( IplImage* frame, IplImage** output )
{
  // TODO: this is only an Object Tracker, it should not return an illustrated image.
  //  instead, it should return a structure of information about tracked object.
  //  There need to be an Illustrator coded.
  //  
  imgSize_ = cvSize(frame->width, frame->height);
  if (first_) {
    temp_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 3);
    mask3C_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 3);
    mask_ = cvCreateImage(imgSize_, IPL_DEPTH_8U, 1);
    first_ = false;
    numStat_ = 100;
    rectsStat_ = new CvRect[numStat_];
    centersStat_ = new CvPoint[numStat_];
  }

  if (*output == NULL) {
    *output = cvCreateImage(imgSize_, IPL_DEPTH_8U, 3);
  }
  cvCopy(frame, *output);
  cvCopy(frame, temp_);
  cvSmooth(temp_, temp_, CV_GAUSSIAN);
  mog_->process((cv::Mat)temp_, (cv::Mat)mask3C_);
  cvCvtColor(mask3C_, mask_, CV_RGB2GRAY);

  cvMorphologyEx(mask_, mask_, 0, 0, CV_MOP_OPEN, openIteration_);
  cvMorphologyEx(mask_, mask_, 0, 0, CV_MOP_CLOSE, closeIteration_);

  // Collect statistics
  if (!rectsStat_)
    rectsStat_ = new CvRect[numStat_];
  if (!centersStat_)
    centersStat_ = new CvPoint[numStat_];

  count_ = numStat_;

  findConnectedComponents(mask_, 0, perimScaleThrhold_, &count_, rectsStat_, centersStat_);
  cvCvtColor(mask_, mask3C_, CV_GRAY2RGB);
  if (currObjs_.empty()) {
    for (int i=0; i<count_; i++) {
      char label[512];
      sprintf(label, "%f", numObjs_);
      ForegroundObject *newObj = new ForegroundObject(label, centersStat_[i]);
      currObjs_.push_back(newObj);
    }
  } else {
    matchObjects(centersStat_, count_);
    for (int i=0; i<currObjs_.size(); i++) {
      drawCross(mask3C_, 
        currObjs_[i]->positionHistory_[currObjs_[i]->positionHistory_.size() - 1], 
        5, cvScalar(255,0,0));
      drawCross(mask3C_, 
        currObjs_[i]->predictedPositionHistory_[currObjs_[i]->predictedPositionHistory_.size() - 1], 
        5, cvScalar(0,255,0));
      drawCross(mask3C_, 
        currObjs_[i]->correctedPositionHistory_[currObjs_[i]->correctedPositionHistory_.size() - 1], 
        5, cvScalar(0,0,255));
    }
  }

  cvNamedWindow("4", CV_WINDOW_NORMAL);
  cvShowImage("4", mask3C_);
}

void printMatrix(double **m, int rows, int cols) {
  printf("\n");
  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols; j++) {
      printf("%10f\t", m[i][j]);
    }
    printf("\n");
  }
}

void ObjectTracker::matchObjects(CvPoint *newCenters, int size) {
  int parameter = imgSize_.width + imgSize_.height;
  int rows = currObjs_.size();
  int cols = size;
  objsM_ = new double*[rows];
  // TODO: clean up memory of objsM_
  for (int i=0; i<rows; i++) {
    objsM_[i] = new double[cols];
  }
  CvPoint *predictedPnts = new CvPoint[rows];
  for (int i=0; i<rows; i++) {
    predictedPnts[i] = currObjs_[i]->predictNextPosition();
  }

  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols; j++) {
      CvPoint distance = subVectors(predictedPnts[i], newCenters[j]);
      objsM_[i][j] = getMagnitude(distance) / parameter;
      if (objsM_[i][j] < matchThreshold_) {
        currObjs_[i]->correctPosition(newCenters[j]);
      }
    }
  }

  printMatrix(objsM_, rows, cols);
}

void ObjectTracker::findConnectedComponents( IplImage* mask, int poly1_hull2 /* = 0 */, double perimScale /* = 0.25 */, int* num /* = NULL */, CvRect* bbs /* = NULL */, CvPoint* centers /* = NULL */ ) {
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