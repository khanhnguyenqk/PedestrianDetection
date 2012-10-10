#include "VideoPlayerUISample.h"


VideoPlayerUISample::VideoPlayerUISample()
{
	int wCam,hCam;
	wCam=4;  //  1280;
	hCam=3;  //960;
	//layout of widgets of interface	
	{   
		int wWin,hWin;
		mainWindow = new Fl_Double_Window(1600, 844, "Video Player");/*
		mainWindow->fullscreen();*/
		mainWindow->user_data((void*)(this));
		wWin=mainWindow->decorated_w();
		hWin=mainWindow->decorated_h();	

		int wZoomSlider,hZoomSlider;
		hZoomSlider=30;
		int wYPanSlider,hYPanSlider;
		wYPanSlider=30;
		int wXPanSlider,hXPanSlider;
		hXPanSlider=30;

		int wView,hView;	
		hView=hWin;

		int wBox,hBox;
		hBox=hView-hZoomSlider-hXPanSlider;
		wBox=int(1.0*wCam/hCam*hBox);
		wView=wBox+wYPanSlider;

		wZoomSlider=wBox; 	hYPanSlider=hBox;  	wXPanSlider=wBox;

		{ VideoView = new Fl_Group(0, 0, wView, hView);
		VideoView->align(Fl_Align(FL_ALIGN_CENTER));
		{
			zoom_fit=new Fl_Button(0,0,wYPanSlider,hZoomSlider,"@<->");
		} // Fl_Button* zoom_fit

		{
			origin_fit=new Fl_Button(0,hZoomSlider+hYPanSlider,
				wYPanSlider,hXPanSlider,"@+");
		}// Fl_Button* origin_fit

		{
			zoom = new Fl_Value_Slider(wYPanSlider, 0, wZoomSlider, 
				hZoomSlider, "Zoom");
			zoom->type(5);
			zoom->selection_color((Fl_Color)1);
			zoom->labeltype(FL_NO_LABEL);
			zoom->labelfont(1);
			zoom->labelsize(12);
			zoom->minimum(0.01);
			zoom->maximum(10);
			zoom->step(0.01);
			zoom->value(1.0);
			zoom->align(Fl_Align(FL_ALIGN_LEFT));
		} // Fl_Slider* zoom
		{ ypan = new Fl_Value_Slider(0, hZoomSlider, wYPanSlider, 
			hYPanSlider, "Y Pan");
		ypan->type(4);
		ypan->selection_color((Fl_Color)4);
		ypan->labeltype(FL_NO_LABEL);
		ypan->labelsize(12);
		ypan->minimum(-1.0);
		ypan->maximum(1.0);
		ypan->step(0.01);
		ypan->value(0.0);
		ypan->align(Fl_Align(FL_ALIGN_LEFT));
		} // Fl_Slider* ypan
		{
			xpan = new Fl_Value_Slider(wYPanSlider, hView-hXPanSlider, 
				wXPanSlider, hXPanSlider, "X pan");
			xpan->type(5);
			xpan->selection_color((Fl_Color)4);
			xpan->labeltype(FL_NO_LABEL);
			xpan->labelfont(1);
			xpan->labelsize(12);
			xpan->minimum(-1.0);
			xpan->maximum(1.0);
			xpan->step(0.01);
			xpan->value(0.0);
			xpan->align(Fl_Align(FL_ALIGN_LEFT));
		} // Fl_Slider* xpan

		{
			glView = new VW_Marker(wYPanSlider,hZoomSlider,wBox,hBox);
			glView->selection_color((Fl_Color)10);
		}
		VideoView->end();
		} // Fl_Group* VideoView

		int wBtnPannel,hBtnPannel;
		wBtnPannel=wWin-wView;
		hBtnPannel=wBtnPannel*0.3;

		int mBtn=3,nBtn=2;
		int wBtn,hBtn;
		wBtn=wBtnPannel/mBtn*0.8;
		hBtn=hBtnPannel/nBtn*0.8;
		int dwBtn,dhBtn;
		dwBtn=(wBtnPannel-wBtn*mBtn)/(mBtn+1.0);
		dhBtn=(hBtnPannel-hBtn*nBtn)/(nBtn+1.0);
		{ grpCommandBtn = new Fl_Group(wWin-wBtnPannel, hWin-hBtnPannel, 
			wBtnPannel, hBtnPannel);
		grpCommandBtn->box(FL_FLAT_BOX);
		grpCommandBtn->color((Fl_Color)33);
		grpCommandBtn->align(Fl_Align(FL_ALIGN_BOTTOM_RIGHT));
		{ btnFileSrc = new Fl_Light_Button(wWin-wBtnPannel+1.0*wBtn+2.0*dwBtn, 
			hWin-hBtnPannel+1.0*dhBtn, wBtn, hBtn, "Open File...");

		btnFileSrc->labelfont(1);
		btnFileSrc->labelsize(20);
		btnFileSrc->selection_color((Fl_Color)2);
		} // Fl_Button* btnFileSrc
		grpCommandBtn->end();
		} // Fl_Group* grpCommandBtn

		//set the callback functions

		btnFileSrc->callback((Fl_Callback*)cb_FileSrc);

		zoom->callback((Fl_Callback*)cb_zoom);
		xpan->callback((Fl_Callback*)cb_xpan);
		ypan->callback((Fl_Callback*)cb_ypan);
		zoom_fit->callback( (Fl_Callback*)cb_zoom_fit);
		origin_fit->callback( (Fl_Callback*)cb_origin_fit);

		mainWindow->end();
		 } // Fl_Double_Window* mainWindow
}


VideoPlayerUISample::~VideoPlayerUISample()
{
}

void VideoPlayerUISample::cb_zoom_i(Fl_Value_Slider * o, void * v)
{
	this->glView->zoomRatio_=o->value();
	this->glView->redraw();
}
void VideoPlayerUISample::cb_zoom(Fl_Value_Slider * o, void * v)
{
	((VideoPlayerUISample*)(o->parent()->parent()->user_data()))->cb_zoom_i(o,v);	
}

void VideoPlayerUISample::cb_xpan_i(Fl_Value_Slider* o, void*) 
{
	this->glView->xPanRatio_=o->value();
	this->glView->redraw();
}
void VideoPlayerUISample::cb_xpan(Fl_Value_Slider* o, void* v) 
{
	((VideoPlayerUISample*)(o->parent()->parent()->user_data()))->cb_xpan_i(o,v);
}

void VideoPlayerUISample::cb_ypan_i(Fl_Value_Slider* o, void*) 
{
	this->glView->yPanRatio_=o->value();
	this->glView->redraw();
}
void VideoPlayerUISample::cb_ypan(Fl_Value_Slider* o, void* v) 
{
	((VideoPlayerUISample*)(o->parent()->parent()->user_data()))->cb_ypan_i(o,v);
}

void VideoPlayerUISample::cb_zoom_fit_i(Fl_Button* o, void* v)
{
	this->glView->zoomRatio_=1.0;
	this->glView->play();
	this->zoom->value(1.0);
	this->glView->redraw();
}

void VideoPlayerUISample::cb_zoom_fit(Fl_Button* o, void* v)
{
	((VideoPlayerUISample*)(o->parent()->parent()->user_data()))->cb_zoom_fit_i(o,v);
}

void VideoPlayerUISample::cb_origin_fit_i(Fl_Button* o, void* v)
{
	this->glView->xPanRatio_=0.0;
	this->glView->yPanRatio_=0.0;
	this->xpan->value(0.0);
	this->ypan->value(0.0);
	this->glView->redraw();
}

void VideoPlayerUISample::cb_origin_fit(Fl_Button* o, void* v)
{
	((VideoPlayerUISample*)(o->parent()->parent()->user_data()))->cb_origin_fit_i(o,v);
}

void VideoPlayerUISample::cb_FileSrc_i(Fl_Light_Button* o, void* v)
{

	

}

void VideoPlayerUISample::cb_FileSrc(Fl_Light_Button* o, void* v)
{
	((VideoPlayerUISample*)(o->parent()->parent()->user_data()))->cb_FileSrc_i(o,v);
}