#include "StdAfx.h"
#include "ObjectTracker.h"

// TODO: what the fuck are these messy functions. Fix them.
void printMatrix(double **m, int rows, int cols) {
  printf("\n");
  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols; j++) {
      printf("%10f\t", m[i][j]);
    }
    printf("\n");
  }
}
// TODO: what the fuck are these messy functions. Fix them.
void drawCross(IplImage *img, CvPoint center, int size, CvScalar color, int thickness = 1) {
  cvLine(img, cvPoint(center.x+size, center.y+size),
    cvPoint(center.x-size, center.y-size), color, 1);
  cvLine(img, cvPoint(center.x+size, center.y-size),
    cvPoint(center.x-size, center.y+size), color, 1);
}
// TODO: what the fuck are these messy functions. Fix them.
void drawText(IplImage *img, const char *text, CvPoint at, CvScalar color, CvFont *font = NULL) {
  if (!font) {
    CvFont font1;
    cvInitFont(&font1, CV_FONT_HERSHEY_DUPLEX, 1.0, 0.8, 0.2, 1, 8);
    cvPutText(img, text, at, &font1, color);
  } else {
    cvPutText(img, text, at, font, color);
  }
}

ObjectTracker::ObjectTracker(CvSize imgSize)
{
  openIteration_ = 1;
  closeIteration_ = 2;
  first_ = true;
  mog_ = new ExLBMixtureOfGaussians;
  numStat_ = 100;
  numObjs_ = 0;
  perimScaleThrhold_ = 0.05;
  matchThreshold_ = 0.09;
}


ObjectTracker::~ObjectTracker(void)
{
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

  matchObjects(centersStat_, count_);
  for (int i=0; i<currObjs_.size(); i++) {
    drawText(mask3C_, currObjs_[i].label_.c_str(), 
      currObjs_[i].positionHistory_[currObjs_[i].positionHistory_.size() - 1],
      cvScalar(255,255,0));
    drawCross(mask3C_, 
      currObjs_[i].positionHistory_[currObjs_[i].positionHistory_.size() - 1], 
      5, cvScalar(255,0,0));
    drawCross(mask3C_, 
      currObjs_[i].predictedPositionHistory_[currObjs_[i].predictedPositionHistory_.size() - 1], 
      5, cvScalar(0,255,0));
    drawCross(mask3C_, 
      currObjs_[i].correctedPositionHistory_[currObjs_[i].correctedPositionHistory_.size() - 1], 
      5, cvScalar(0,0,255));
  }

  cvNamedWindow("4", CV_WINDOW_NORMAL);
  cvShowImage("4", mask3C_);
}

void ObjectTracker::matchObjects(CvPoint *newCenters, int size) {
  if (currObjs_.empty()) {
    for (int i=0; i<size; i++) {
      createNewObject(newCenters[i]);
    }
    return;
  }

  if (size == 0) {
    removeObjects();
  }

  int parameter = imgSize_.width + imgSize_.height;
  int rows = currObjs_.size();
  int cols = size;
  objsM_ = new double*[rows];
  
  for (int i=0; i<rows; i++) {
    objsM_[i] = new double[cols];
  }
  CvPoint *predictedPnts = new CvPoint[rows];
  for (int i=0; i<rows; i++) {
    predictedPnts[i] = currObjs_[i].predictNextPosition();
  }

  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols; j++) {
      CvPoint distance = subVectors(predictedPnts[i], newCenters[j]);
      objsM_[i][j] = getMagnitude(distance) / parameter;
    }
  }

  vector<int> list;
  // Check columns
  for (int j=0; j<cols; j++) {
    list.clear();
    for (int i=0; i<rows; i++) {
      if (objsM_[i][j] < matchThreshold_) {
        list.push_back(i);
      }
    }
    switch (list.size()) {
    case 0: // New object
      createNewObject(newCenters[j]);
      break;
    case 1:
      break;
    default:
      break;
    }
  }

  // Check rows
  vector<int> removedObjectsv;
  removedObjectsv.clear();
  for (int i=0; i<rows; i++) {
    list.clear();
    for (int j=0; j<cols; j++) {
      if (objsM_[i][j] < matchThreshold_) {
        list.push_back(j);
      }
    }

    switch(list.size()) {
    case 0: // No more match
      removedObjectsv.push_back(i);
      break;
    case 1:
      currObjs_[i].correctPosition(newCenters[list[0]]);
      break;
    default:
      break;
    }
  }

  removeObjects(&removedObjectsv);

  
  //printMatrix(objsM_, rows, cols);
  for (int i=0; i<rows; i++) {
    delete[] objsM_[i];
  }
  delete[] objsM_;
}

void ObjectTracker::removeObjects(vector<int> *iterators /* = NULL */) {
  // remove all
  if (!iterators) {
    currObjs_.clear();
  } else {
    for (int i=0; i<iterators->size(); i++) {
      currObjs_[(*iterators)[i]].positionHistory_.clear();
    }
    for (vector<ForegroundObject>::iterator i=currObjs_.begin(); i!=currObjs_.end();) {
      if (!(*i).positionHistory_.size()) {
        i = currObjs_.erase(i);
      } else {
        i++;
      }
    }
  }
}

void ObjectTracker::createNewObject(CvPoint center) {
  char label[512];
  sprintf(label, "%d", numObjs_);
  numObjs_++;
  ForegroundObject newObj(label, center);
  currObjs_.push_back(newObj);
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