#include "StdAfx.h"
#include "AOI_Processor.h"

AoiProcessWindow::~AoiProcessWindow(void)
{
	releaseImageVector(extractedAois_);

	for (unsigned i=0; i<windowNames_.size(); i++) {
		delete[] windowNames_[i];
	}
	windowNames_.clear();
	cvDestroyAllWindows();
}

int AoiProcessWindow::handle(int event) {
	if (!videoInitiated_)
		return VideoWindow::handle(event);

	if (event == FL_PUSH) {
		if (Fl::event_button() == FL_MIDDLE_MOUSE) {
			saveScreen();
			return 1;
		}
	}
	return VideoWindowMarker::handle(event);
}

void AoiProcessWindow::subwindowManage(int num) {
	int numWindows = windowNames_.size();
	// Create more if needed
	if (num > numWindows) {
		int i = numWindows;
		for (;i < num; i++) {
			char *name;
			name = new char[128];
			sprintf(name, "Window %05d", i);
			windowNames_.push_back(name);
			cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
		}
	}
	// Destroy if have to
	else if (num < numWindows) {
		int i = num;
		for (;i < numWindows; i++) {
			cvDestroyWindow(windowNames_[i]);
			delete[] windowNames_[i];
		}
		windowNames_.erase(windowNames_.begin()+num, windowNames_.end());
	}
}

void AoiProcessWindow::motionDetectorManage(int size) {
	int numDetector = motionDetectors_.size();
	
	// If changed, remove all. Create brand new.
	if (size != numDetector) {
		clearMotionDetectorsMemories();
		// Create more if needed
		for (int i=0; i < size; i++) {
			ObjectTracker *m = new ObjectTracker(cvSize(extractedAois_[i]->width, extractedAois_[i]->height));
			motionDetectors_.push_back(m);
		}
	}
}

void AoiProcessWindow::drawPictureOnSubwindows(vector<IplImage*> images) {
	subwindowManage(images.size());
	if (images.size() != windowNames_.size()) {
		/*fl_alert("Number of cropped areas does not match number on windows created.");*/
		return;
	}
	for (unsigned i=0; i<images.size(); i++) {
    cvNamedWindow(windowNames_[i], CV_WINDOW_NORMAL);
		cvShowImage(windowNames_[i], images[i]);
	}
}

void AoiProcessWindow::releaseImageVector(vector<IplImage*> &images) {
	for (unsigned i=0; i<images.size(); i++) {
		cvReleaseImage(&images[i]);
	}
	images.clear();
}

void AoiProcessWindow::draw() {
	if (videoInitiated_) {
		if ((playStatus_ == PLAY)) {
			currFrame_ = cvQueryFrame(videoCapture_);

			drawBlackScreen();
			if (currFrame_ == NULL)
				stop();
			else {
				cloneAndDrawTools();
				drawImageOnMainWindow(clone_);
				//if (extract_) {
    //      // TODO: change this to run the video and save AOI into smaller videos.
    //      //  No need to track motion in every AOI. The process is very resource consuming.
				//	vector<IplImage*> motionTracked;
				//	extractedAois_ = extractAOI(currFrame_, aois_);
				//	trackMotionAndIllustrate(extractedAois_, motionTracked);
				//	drawPictureOnSubwindows(motionTracked);
				//	releaseImageVector(extractedAois_);
				//	releaseImageVector(motionTracked);
				//} 
				// TODO: 
				if (!motionDetector_) {
					motionDetector_ = new ObjectTracker(cvSize(currFrame_->width,
											currFrame_->height));
					cvNamedWindow("Tracked", CV_WINDOW_NORMAL);
				} else {
					IplImage *ret = NULL;
          motionDetector_->processImage(currFrame_, &ret);
          cvNamedWindow("Tracked", CV_WINDOW_NORMAL);
					cvShowImage("Tracked", ret);
					cvReleaseImage(&ret);
          analyzeObjects(motionDetector_->currObjs_);
				}
			}
		} else if ((playStatus_ == PAUSE)) {
			drawBlackScreen();
			if (clone_ == NULL)
				stop();
			else {
				if (cloneDone_) {
					cloneAndDrawTools();
					drawImageOnMainWindow(clone_);
				}
				else {
					drawAllTools(currFrame_);
					drawImageOnMainWindow(currFrame_);
				}
			}
		} else if (playStatus_ == STOP)
			drawBlackScreen();
	}
	updateDependences();
}

void AoiProcessWindow::analyzeObjects(vector<ForegroundObject> objs) {
  if (!pastObjects_.size()) {
    for (int i=0; i<objs.size(); i++) {
      pastObjects_.push_back(objs[i]);
    }
  } else {
    for (int i=0; i<pastObjects_.size(); i++) {
      bool match = false;
      for (int j=0; j<objs.size(); j++) {
        if (pastObjects_[i].isEqual(objs[j])) {
          match = true;
          CvPoint lastPosition = *(pastObjects_[i].correctedPositionHistory_.end() - 1);
          CvPoint newPosition = *(objs[j].correctedPositionHistory_.end() - 1);

          // Lines
          try {
            
            for (int k=0; k<lines_.size(); k++) {
              LineSegment2D line(fromCvPoint(lines_[k]->p1_), fromCvPoint(lines_[k]->p2_));
              LineSegment2D trajectory(fromCvPoint(lastPosition), fromCvPoint(newPosition));
              try {
                findIntersection(line, trajectory);
                printf("Object %s passes line %d\n", objs[j].label_.c_str(), k);
              } catch (...) {}
            }
          } catch (...) {}

          // AOIS
          for (int k=0; k<aois_.size(); k++) {
            if (useRect_) {
              if (aois_[k]->doesContainPoint(lastPosition) && !aois_[k]->doesContainPoint(newPosition))
                printf("Object %s leaves AOI %d\n", objs[j].label_.c_str(), k);
              if (!aois_[k]->doesContainPoint(lastPosition) && aois_[k]->doesContainPoint(newPosition))
                printf("Object %s enters AOI %d\n", objs[j].label_.c_str(), k);
            } else {
              if (((AoiTrapezium*)aois_[k])->doesContainPoint(lastPosition) 
                && !((AoiTrapezium*)aois_[k])->doesContainPoint(newPosition))
                printf("Object %s leaves AOI %d\n", objs[j].label_.c_str(), k);
              if (!((AoiTrapezium*)aois_[k])->doesContainPoint(lastPosition) 
                && ((AoiTrapezium*)aois_[k])->doesContainPoint(newPosition))
                printf("Object %s enters AOI %d\n", objs[j].label_.c_str(), k);
            }
          }

        }
      }
    }
    pastObjects_.clear();
    for (int i=0; i<objs.size(); i++) {
      pastObjects_.push_back(objs[i]);
    }
  }
}

void AoiProcessWindow::clearMotionDetectorsMemories() {
	int numDetector = motionDetectors_.size();
	for (int i = 0; i<numDetector; i++) {
		delete motionDetectors_[i];
	}
	motionDetectors_.clear();
}

void AoiProcessWindow::trackMotionAndIllustrate(vector<IplImage*> src, vector<IplImage*> &dst) 
{
	motionDetectorManage(src.size());
	for (unsigned i=0; i<src.size(); i++) {
		IplImage *img;
    motionDetectors_[i]->processImage(src[i], &img);
		dst.push_back(img);
	}
}

bool AoiProcessWindow::saveScreen() {
	if ((clone_ == NULL) && (currFrame_ == NULL))
		return false;
	if (playStatus_==PAUSE) {
		if (clone_)
			saveMarkedImage(clone_);
		else
			return false;
	}
	else if (playStatus_==PLAY) {
		if (currFrame_)
			saveMarkedImage(currFrame_);
		else
			return false;
	}
	else
		return false;
	return true;
}

vector<IplImage*> AoiProcessWindow::extractAOI(IplImage *image, vector<AreaOfInterest*> aois) {
	if (useRect_) {
		vector<IplImage*> ret;
		int s = aois.size();

		for (int i = 0; i<s; i++) {
			cvSetImageROI(image, aois[i]->getRect());
			ret.push_back(cvCreateImage(cvGetSize(image), image->depth, image->nChannels));
			cvCopy(image, ret[i], NULL);
			cvResetImageROI(image);
		}

		return ret;
	}
	else {
		vector<IplImage*> ret;
		int s = aois.size();
		LineSegment2D lines[4];
		CvPoint2D32f relTopLeft;

		for (int i = 0; i<s; i++) {
			// Get Rect part
			cvSetImageROI(image, aois[i]->getRect());
			ret.push_back(cvCreateImage(cvGetSize(image), image->depth, image->nChannels));
			cvCopy(image, ret[i], NULL);
			// Clean Trapezium part
			relTopLeft = cvPoint2D32f(aois[i]->getRect().x, aois[i]->getRect().y);
			lines[0] = findLineSegmentFormular2D(subVectors(aois[i]->getPoint(0), relTopLeft)
				, subVectors(aois[i]->getPoint(1), relTopLeft));
			lines[1] = findLineSegmentFormular2D(subVectors(aois[i]->getPoint(1), relTopLeft)
				, subVectors(aois[i]->getPoint(2), relTopLeft));
			lines[2] = findLineSegmentFormular2D(subVectors(aois[i]->getPoint(2), relTopLeft)
				, subVectors(aois[i]->getPoint(3), relTopLeft));
			lines[3] = findLineSegmentFormular2D(subVectors(aois[i]->getPoint(3), relTopLeft)
				, subVectors(aois[i]->getPoint(0), relTopLeft));
			for (int y = 0; y < ret[i]->height; y++) {
				int x[4]; 
				x[0] = -1; x[1] = -1; x[2] = -1; x[3] = -1;
				try { x[0] = (int)findX(lines[0], y);} catch (...) {}
				try { x[1] = (int)findX(lines[1], y);} catch (...) {}
				try { x[2] = (int)findX(lines[2], y);} catch (...) {}
				try { x[3] = (int)findX(lines[3], y);} catch (...) {}
				int low = -1, high = -1;
				for (int j = 0; j < 4; j++) {
					if (x[j] != -1) {
						if (low == -1)
							low = x[j];
						else {
							if ((low == x[j] && high == -1) || (x[j] > low))
								high = x[j];
							else if (x[j] < low) {
								high = low;
								low = x[j];
							}
						}
					}
				}
				setElements(&ret[i], cvPoint(0, y), cvPoint(low, y), (uchar)0);
				setElements(&ret[i], cvPoint(high, y), cvPoint(ret[i]->width - 1, y), (uchar)0);
			}
			cvResetImageROI(image);
		}

		return ret;
	}
}

void AoiProcessWindow::saveMarkedImage(IplImage* image) {
	string directoryName = VideoWindowMarker::getSaveDirectory(videoName_);
	bool diretoryExists = false;

	struct stat st;
	if (stat(directoryName.c_str(), &st) == 0)
		diretoryExists = true;
	else
	{
		int mkdirResult = _mkdir(directoryName.c_str());
		if (mkdirResult == 0)
			diretoryExists = true;
		else
		{
			char errMsg[256];
			sprintf(errMsg, "The directory creation failed with error: %d. Cannot save image\n", mkdirResult);
			fl_alert(errMsg);
		}
	}

	if (diretoryExists) {
		int frameNum = (int)cvGetCaptureProperty(videoCapture_, CV_CAP_PROP_POS_FRAMES);
		char filePath[512];
		sprintf(filePath, "%s\\%010d.jpg", directoryName.c_str(), frameNum);
		cvSaveImage(filePath, image);
		// Save ts areas
		vector<IplImage*> aois = extractAOI(currFrame_, aois_);
		for (unsigned i=0; i<aois.size(); i++)
		{
			sprintf(filePath, "%s\\%010d_%05d.jpg", directoryName.c_str(), frameNum, i);
			cvSaveImage(filePath, aois[i]);
			cvReleaseImage(&aois[i]);
		}
		aois.clear();
	}
}

void AoiProcessWindow::setElements(IplImage **image, CvPoint from, CvPoint to, uchar v) {
	// Check some conditions
	if (from.x == to.x && from.y == to.y)
		return;
	if (!(from.x <= to.x && from.y <= to.y))
		return;
	int w = (*image)->width;
	int h = (*image)->height;
	if (!((from.x < w) && (from.y < h) && (to.x < w) && (to.y < h)))
		return;
	
	// Pass all conditions.
	for (int y = from.y; y <= to.y; y++) {
		uchar *ptr = (uchar*)((*image)->imageData + y * (*image)->widthStep);;
		int begin, end;
		if (y == from.y)
			begin = from.x; 
		else
			begin = 0;
		if (y == to.y)
			end = to.x;
		else
			end = (*image)->width - 1;

		for (int x = begin; x <= end; x++) {
			ptr[3*x] = v;
			ptr[3*x + 1] = v;
			ptr[3*x + 2] = v;
		}
	}
}