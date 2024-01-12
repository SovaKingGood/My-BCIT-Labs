#include "stdafx.h"
#include "CPong.h"
#include "cvui.h"
#include "CControlPi.h"
#include <chrono>
#include <ratio>
#include <thread>
#include <numeric>

CPong::CPong(cv::Size size, int comid)
{	_Canvas_size = size;
	_canvas = cv::Mat::zeros(_Canvas_size, CV_8UC3);
	cvui::init("Pong");
	_exit = 0;
	_Position = cv::Point(_Canvas_size.width / 2, _Canvas_size.height/2);

	if (rand() % 2) {
		_Velocity.x = 1;
	}
	else
	{
		_Velocity.x = -1;
	}
	if (rand() % 2) {
		_Velocity.y = 1;
	}
	else
	{
		_Velocity.y = -1;
	}


	color = cv::Scalar(255, 255, 255);
	_radius = 20;
	_speed_multiplier = 250;
	_fps_current = 30;
	_deltaT = 1/30;
	_Paddle_Position_p1 = cv::Point(_canvas.cols-20, _canvas.rows / 2);
	_Paddle_Position_p1_last = cv::Point(_canvas.cols - 20, _canvas.rows / 2);
	_analog_y = 2000;
	_score_p1 = 0;
	_score_p2 = 0;
	_Paddle_Position_p2 = cv::Point(10, _canvas.rows / 2);
	_Paddle_Position_p2_last = cv::Point(10, _canvas.rows / 2);
	_thread_exit = false;
	_fps_average = 30;
	_game_over = false;
}

CPong::~CPong()
{
	cv::destroyAllWindows();

}

void CPong::update()
{

	//deltaT_mutex.lock();
	_Paddle_Position_p1_last = _Paddle_Position_p2;
	_Paddle_Position_p2_last = _Paddle_Position_p2;
	if (_reset == true) {

		_Position = cv::Point(_Canvas_size.width/2 , _Canvas_size.height/2);
		_reset = false;

		if ((rand() % 2)) {
			_Velocity.x = 1;
		}
		else
		{
			_Velocity.x = -1;
		}

		if (rand() % 2) {
			_Velocity.y = 1;
		}
		else
		{
			_Velocity.y = -1;
		}
	}


	_Position.x += _Velocity.x * (_deltaT * _speed_multiplier);
	_Position.y += _Velocity.y * (_deltaT * _speed_multiplier);

	CControl.get_analog(1, _analog_y);
    _analog_y = -1*_analog_y + 1024;

	if (_analog_y > 2000)//up
	{
		_Paddle_Position_p1.y = _Paddle_Position_p1.y - (abs(_analog_y - 2000) / 100);
	}
	if (_analog_y < 1900)//down
	{
		_Paddle_Position_p1.y = _Paddle_Position_p1.y + (abs(_analog_y - 1900) / 100);

	}


	if (_Paddle_Position_p1.y + 100 > _canvas.rows) {

		_Paddle_Position_p1.y = _canvas.rows - 100;
	}
	if (_Paddle_Position_p1.y < 0) {

		_Paddle_Position_p1.y = 0;
	}

	if (_Paddle_Position_p2.y + 50 < _Position.y - 10) {

		_Paddle_Position_p2.y += 10;
	}
	if (_Paddle_Position_p2.y + 50 > _Position.y + 10)
	{
		_Paddle_Position_p2.y -= 10;
	}

	if (_Paddle_Position_p2.y + 100 > _canvas.rows) {

		_Paddle_Position_p2.y = _canvas.rows - 100;
	}
	if (_Paddle_Position_p2.y < 0) {

		_Paddle_Position_p2.y = 0;
	}








	if (_Position.inside(cv::Rect(_Paddle_Position_p1.x - _radius, _Paddle_Position_p1.y - _radius, 15 + _radius, 115 + _radius))) {//right paddle
		_Velocity.x = -1;
	}
	if (_Position.inside(cv::Rect(_Paddle_Position_p2.x, _Paddle_Position_p2.y, 15 + _radius, 115 + _radius))) { //left paddle

		_Velocity.x = 1;
	}

	//deltaT_mutex.unlock();

	if (_Position.x + _radius > _Canvas_size.width) {
		_score_p2++;
		if (_score_p2 == 5) {
			_game_over = true;
			while (_reset != false) {

			}

		}
		else
		{
			_reset = true;
		}
	}
	if (_Position.x - _radius < 0) {
		_score_p1++;
		if (_score_p1 == 5) {
			_game_over = true;
			while (_reset != false) {

			}

		}
		else
		{
			_reset = true;
		}


	}

	if (_Position.y + _radius > _Canvas_size.height) {
		_Velocity.y = -1;
	}
	if (_Position.y - _radius < 0) {
		_Velocity.y = 1;

	}

	cv::waitKey(1);





}

void CPong::draw()
{


	auto calc_start = std::chrono::steady_clock::now();
	auto end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(30); // 0.033 seconds = 33.333 milli = 33333.3 us

	_fps_current = 1 / _deltaT;


	_fps_vector.push_back(_fps_current);
	if (_fps_vector.size() > 20) {
		_fps_vector.erase(_fps_vector.begin());
	}


	_fps_average = 0;
	for (auto i = _fps_vector.begin(); i != _fps_vector.end(); i++) {

		_fps_average += *i;

	}
	_fps_average = _fps_average / _fps_vector.size();

	//deltaT_mutex.unlock();

	std::cout << _fps_average << "\n";




	_canvas = cv::Mat::zeros(_Canvas_size, CV_8UC3);

	// Gui Menu



	if (_game_over == true) {

		gui = cv::Point(_canvas.rows/2 - 300, _canvas.cols/2 - 300);//top left
		if (_score_p1 == 5) {

			cvui::printf(_canvas, gui.x, gui.y, 1, 0xFFFFFF, "You win ! :D Click reset to play again !");
		}
		else
		{

			cvui::printf(_canvas, gui.x, gui.y, 1, 0xFFFFFF, "You lost ! :-( Click reset to play again !");
		}
		gui += cv::Point(_canvas.rows / 2, _canvas.cols / 2);
		if (cvui::button(_canvas, gui.x, gui.y, 60, 30, "Reset"))// check for exit button
		{
			_score_p1 = 0;
			_score_p2 = 0;
			_reset = true;
			_game_over = false;
		}
	}
	else
	{
		gui = cv::Point(30, 10);//top left
		cvui::window(_canvas, gui.x, gui.y, 200, 200, "PONG by Brennan Pinette");

		gui += cv::Point(30, 30);
		cvui::text(_canvas, gui.x, gui.y, "Radius");
		gui += cv::Point(-30, 0);std::cout << "1";

		gui += cv::Point(130, 0);
		cvui::text(_canvas, gui.x, gui.y, "Speed");
		gui += cv::Point(-130, 0);

		//deltaT_mutex.lock();
		gui += cv::Point(500, 0);
		cvui::printf(_canvas, gui.x, gui.y, 1, 0xFFFFFF, "FPS: %.0f", _fps_average);
		gui += cv::Point(-500, 0);

		gui += cv::Point(0, 20);
		cvui::trackbar(_canvas, gui.x, gui.y, 100, &_radius, 5, 100, 1);

		gui += cv::Point(100, 0);
		cvui::trackbar(_canvas, gui.x, gui.y, 100, &_speed_multiplier, 100, 400, 0.5);
		gui += cv::Point(-100, 0);


		gui += cv::Point(0, 50);
		cvui::printf(_canvas, gui.x, gui.y, 0.5, 0xFFFFFF, "Player Score %d", _score_p1);

		gui += cv::Point(0, 20);
		cvui::printf(_canvas, gui.x, gui.y, 0.5, 0xFFFFFF, "AI Score %d", _score_p2);



		gui += cv::Point(30, 30);
		if (cvui::button(_canvas, gui.x, gui.y, 60, 30, "Exit"))// check for exit button
		{
			_exit = true;
		}std::cout << "1";


		gui += cv::Point(75, 0);


		if (cvui::button(_canvas, gui.x, gui.y, 60, 30, "Reset"))// check for exit button
		{
			_score_p1 = 0;
			_score_p2 = 0;
			_reset = true;
		}



		if (_reset_button_state == 1)
		{
			_reset = true;
			_reset_button_state = false;
		}
		cvui::update();


		cv::circle(_canvas, _Position, _radius, cv::Scalar(color), -1);
		cv::rectangle(_canvas, cv::Rect(_Paddle_Position_p1.x, _Paddle_Position_p1.y, 10, 100), cv::Scalar(255, 255, 255), -1, 8);
		cv::rectangle(_canvas, cv::Rect(_Paddle_Position_p2.x, _Paddle_Position_p2.y, 10, 100), cv::Scalar(255, 255, 255), -1, 8);

	}




	//deltaT_mutex.unlock();

	cv::imshow("Pong", _canvas);

	cv::waitKey(1);

	//deltaT_mutex.lock();
	std::this_thread::sleep_until(end_time);
	auto calc_end = std::chrono::steady_clock::now();

	std::chrono::duration<double, std::milli> calc_elapsed = (calc_end - calc_start);

	_deltaT = (calc_elapsed.count() / 1000);



}

void CPong::run()
{

	std::thread t1(&CPong::update_thread, this);
	std::thread t2(&CPong::draw_thread, this);


	do
	{

		//update();
		//draw();


	} while ((cv::waitKey(1) != 'q') && (_exit != true));
	_thread_exit = true;


	t1.join();
	t2.join();

}

void CPong::update_thread(CPong* ptr)
{
	while (ptr->_thread_exit == false){
		ptr->update();

	}
}

void CPong::draw_thread(CPong* ptr)
{
	while (ptr->_thread_exit == false) {
		ptr->draw();

	}
}
