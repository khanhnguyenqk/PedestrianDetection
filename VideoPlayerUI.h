// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef VideoPlayerUI_h
#define VideoPlayerUI_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>
#include "AOI_Processor.h"
#include <FL/Fl_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Input.H>
#include <FL/Fl_Multiline_Output.H>
#include <opencv2/opencv.hpp>

class VideoPlayerUI {
public:
  VideoPlayerUI();
  Fl_Double_Window *mainWindow_;
  Fl_Value_Slider *xPan_;
  Fl_Value_Slider *yPan_;
  Fl_Value_Slider *zoom_;
  AOI_ProcessorWindow *aoiProcessorWindow;
  Fl_Slider *slider_;
  Fl_Button *stop_;
  Fl_Button *play_;
  Fl_Button *halfSpeed_;
  Fl_Button *doubleSpeed_;
  Fl_Button *prevFrame_;
  Fl_Button *nextFrame_;
  Fl_Button *defaultView;
  Fl_Multiline_Output *videoDetails_;
  Fl_File_Input *filePathInput_;
  Fl_Button *openFile_;
  Fl_Button *defaultSetting_;

  // Draw tools
  Fl_Button *newRect_;
  Fl_Button *deleteRect_;
  Fl_Button *nextRect_;
  Fl_Button *prevRect_;
  Fl_Button *savePicture_;

  // Call back
  void cb_play_i(Fl_Button* obj, void* v);
  static void cb_play(Fl_Button* obj, void* v);

  void cb_stop_i(Fl_Button* obj, void* v);
  static void cb_stop(Fl_Button* obj, void* v);

  void cb_doubleSpeed_i(Fl_Button* obj, void* v);
  static void cb_doubleSpeed(Fl_Button* obj, void* v);

  void cb_halfSpeed_i(Fl_Button* obj, void* v);
  static void cb_halfSpeed(Fl_Button* obj, void* v);

  void cb_prevFrame_i(Fl_Button* obj, void* v);
  static void cb_prevFrame(Fl_Button* obj, void* v);

  void cb_nextFrame_i(Fl_Button* obj, void* v);
  static void cb_nextFrame(Fl_Button* obj, void* v);

  void cb_zoom_i(Fl_Value_Slider* obj, void* v);
  static void cb_zoom(Fl_Value_Slider* obj, void* v);

  void cb_xPan_i(Fl_Value_Slider* obj, void* v);
  static void cb_xPan(Fl_Value_Slider* obj, void* v);

  void cb_yPan_i(Fl_Value_Slider* obj, void* v);
  static void cb_yPan(Fl_Value_Slider* obj, void* v);

  void cb_zoom_fit_i(Fl_Button* obj, void* v); 
  static void cb_zoom_fit(Fl_Button* obj, void* v);

  void cb_slider_i(Fl_Slider* obj, void* v);
  static void cb_slider(Fl_Slider* obj, void* v);

  void cb_origin_fit_i(Fl_Button*, void*);
  static void cb_origin_fit(Fl_Button*, void*);

  void cb_openFile_i(Fl_Button* obj, void* v);
  static void cb_openFile(Fl_Button* obj, void* v);

  void cb_newRect_i(Fl_Button* obj, void* v);
  static void cb_newRect(Fl_Button* obj, void* v);

  void cb_delRect_i(Fl_Button* obj, void* v);
  static void cb_delRect(Fl_Button* obj, void* v);

  void cb_nextRect_i(Fl_Button* obj, void* v);
  static void cb_nextRect(Fl_Button* obj, void* v);

  void cb_prevRect_i(Fl_Button* obj, void* v);
  static void cb_prevRect(Fl_Button* obj, void* v);

  void cb_default_i(Fl_Button* obj, void* v);
  static void cb_default(Fl_Button* obj, void* v);

  void cb_save_i(Fl_Button* obj, void* v);
  static void cb_save(Fl_Button* obj, void* v);
  // Others
public:
	void setFilePath(const char* filePath);
};
#endif
