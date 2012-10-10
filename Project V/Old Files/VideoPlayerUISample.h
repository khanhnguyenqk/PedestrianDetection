#ifndef VIDEO_PLAYER_UI_H_
#define VIDEO_PLAYER_UI_H_
#include <fl/fl.H>
#include <fl/Fl_Double_Window.H>
#include <fl/Fl_Group.H>
#include <fl/Fl_Value_Slider.H>
#include <fl/Fl_Box.H>
#include <fl/Fl_Text_Display.H>
#include <fl/Fl_Widget.H>
#include <fl/Fl_Light_Button.H>
#include <fl/Fl_Widget.H>
#include <fl/fl_ask.H>

#include "VW_Marker.h"


class VideoPlayerUISample
{
public:
	VideoPlayerUISample();
	virtual ~VideoPlayerUISample();

	Fl_Double_Window *mainWindow;

	Fl_Group *VideoView;
	Fl_Value_Slider *zoom;
	Fl_Value_Slider *ypan;
	Fl_Value_Slider *xpan;
	VW_Marker *glView;

	Fl_Button *zoom_fit;
	Fl_Button *origin_fit;

	Fl_Group *grpCommandBtn;
	Fl_Light_Button *btnFileSrc;

private:	
	static void mainWindow_callback(Fl_Widget* widget,void*);

	void cb_FileSrc_i(Fl_Light_Button*, void*);
	static void cb_FileSrc(Fl_Light_Button*, void*);

	void cb_zoom_i(Fl_Value_Slider*, void*);
	static void cb_zoom(Fl_Value_Slider*, void*);

	void cb_xpan_i(Fl_Value_Slider*, void*);
	static void cb_xpan(Fl_Value_Slider*, void*);

	void cb_ypan_i(Fl_Value_Slider*, void*);
	static void cb_ypan(Fl_Value_Slider*, void*);

	void cb_zoom_fit_i(Fl_Button*, void*);
	static void cb_zoom_fit(Fl_Button*, void*);

	void cb_origin_fit_i(Fl_Button*, void*);
	static void cb_origin_fit(Fl_Button*, void*);
};
#endif

