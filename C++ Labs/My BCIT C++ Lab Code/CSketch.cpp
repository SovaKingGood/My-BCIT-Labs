#include "stdafx.h"
#include "CSketch.h"
#include "cvui.h"


enum { BLUE = 1, GREEN, RED };
enum { DIGITAL = 0, ANALOG, SERVO };

CSketch::CSketch(cv::Size size, int comid)
{
	_Canvas_size = size;
	CControl.init_com(comid);
	_canvas = cv::Mat::zeros(_Canvas_size, CV_8UC3);
	cvui::init("Sketch");
	draw_x = _canvas.rows / 2;
	draw_y = _canvas.cols / 2;
	drawer = cv::Point( draw_x, draw_y);//set drawer to center
	drawer_previous = cv::Point(draw_x, draw_y);//set drawer to center
	color = cv::Scalar(0, 0, 255); //set color to red BGR
	color_select = RED;
	CControl.set_data(DIGITAL, 39, 1);//r
	//Sleep(10);
	CControl.set_data(DIGITAL, 38, 0);//g
	//Sleep(10);
	CControl.set_data(DIGITAL, 37, 0);//b

	_L_threshold_x = 1850;
	_L_threshold_y = 1900;
	_U_threshold_x = 2100;
	_U_threshold_y = 2000;

	_draw_speed = 50;
	_exit = 0;
	CControl.get_analog(0, _analog_x); //Channel #1, Horizontal
	CControl.get_analog(1, _analog_y); //Channel #2, Vertical

}

CSketch::~CSketch()
{
	cv::destroyAllWindows();
}

void CSketch::update()
{
	if (_reset==true)
	{
		draw_x = _canvas.rows / 2;
		draw_y = _canvas.cols / 2;
		drawer_previous = drawer;
		drawer = cv::Point(draw_x, draw_y);//set drawer to center
		drawer_previous = cv::Point(draw_x, draw_y);//set drawer to center
		color = cv::Scalar(0, 0, 255); //set color to red BGR
		color_select = RED;
		CControl.set_data(DIGITAL, 39, 1);//r
		CControl.set_data(DIGITAL, 38, 0);//g
		CControl.set_data(DIGITAL, 37, 0);//b
		_canvas = cv::Mat::zeros(_Canvas_size, CV_8UC3);
		_reset = false;
		cv::waitKey(20);
		CControl.get_analog(0, _analog_x); //Channel #1, Horizontal
		CControl.get_analog(1, _analog_y); //Channel #2, Vertical
	}



	CControl.get_analog(0, _analog_x); //Channel #1, Horizontal
	CControl.get_analog(1, _analog_y); //Channel #2, Vertical

	CControl.get_button(32, _color_button_state); // Check Color Switch Button

	if (_color_button_state == 1)
	{

			color_select++;

			if (color_select > 3) {
				color_select = BLUE;
			}
			if (color_select == RED) {
				CControl.set_data(DIGITAL, 39, 1);//r
				CControl.set_data(DIGITAL, 38, 0);//g
				CControl.set_data(DIGITAL, 37, 0);//b
			}
			if (color_select == BLUE) {
				CControl.set_data(DIGITAL, 39, 0);//r
				CControl.set_data(DIGITAL, 38, 0);//g
				CControl.set_data(DIGITAL, 37, 1);//b
			}
			if (color_select == GREEN) {
				CControl.set_data(DIGITAL, 39, 0);//r
				CControl.set_data(DIGITAL, 38, 1);//g
				CControl.set_data(DIGITAL, 37, 0);//b

			}
			cv::waitKey(20);
	}

	if (_analog_x < _L_threshold_x) //left
	{
		draw_x = draw_x - (abs(_analog_x - _L_threshold_x) / _draw_speed);
	}
	if (_analog_x > _U_threshold_x) //right
	{
		draw_x = draw_x + (abs(_analog_x - _U_threshold_x) / _draw_speed);
	}

	if (_analog_y > _U_threshold_y)//up
	{
		draw_y = draw_y - (abs(_analog_y - _U_threshold_y) / _draw_speed);
	}
	if (_analog_y < _L_threshold_y)//down
	{
		draw_y = draw_y + (abs(_analog_y - _L_threshold_x) / _draw_speed);
	}


	if (draw_x > 800)
	{
		draw_x = 800;
	}
	if (draw_x < 0)
	{
		draw_x = 0;
	}
	if (draw_y > 800)
	{
		draw_y = 800;
	}
	if (draw_y < 0)
	{
		draw_y = 0;
	}


}



void CSketch::draw()
{
	// Gui Menu

	gui = cv::Point(10, 10);//top left
	cvui::window(_canvas, gui.x, gui.y, 150, 100, "  Etch - A_Sketch");
	gui += cv::Point(5, 30);
	cvui::text(_canvas, gui.x, gui.y, "By Brennan Pinette");

	gui += cv::Point(0, 15);
	cvui::text(_canvas, gui.x, gui.y, "Current Color:  ");

	gui += cv::Point(95, 0);
	switch (color_select)
	{
	case BLUE:  cvui::text(_canvas, gui.x, gui.y, "BLUE", 0.4, 0x0000ff); color = cv::Scalar(255, 0, 0);  break;//blue [b]
	case GREEN: cvui::text(_canvas, gui.x, gui.y, "GREEN", 0.4, 0x00ff00); color = cv::Scalar(0, 255, 0); break; //green [g]
	case RED:   cvui::text(_canvas, gui.x, gui.y, "RED", 0.4, 0xff0000); color = cv::Scalar(0, 0, 255);   break; //red [r]
	}

	gui += cv::Point(-95, 20);
	if (cvui::button(_canvas, gui.x, gui.y, 60, 30, "Exit"))// check for exit button
	{
		CSketch::_exit = true;
	}


	gui += cv::Point(75, 0);
	if (cvui::button(_canvas, gui.x, gui.y, 60, 30, "Reset"))// check for exit button
	{
		_reset = true;
	}

	cvui::update();

	drawer = cv::Point(draw_x, draw_y);

	cv::line(_canvas, drawer_previous, drawer, cv::Scalar(color), 8);

	drawer_previous = drawer;

	CControl.get_button(33, _reset_button_state); // check reset button
	if (_reset_button_state == 1)
	{
		_reset = true;
		_reset_button_state = false;
	}

	cv::imshow("Sketch", _canvas);
}

void CSketch::run()
{
    do
	{
		update();
		draw();

	} while ((cv::waitKey(1) != 'q') && (CSketch::_exit != true));





}
