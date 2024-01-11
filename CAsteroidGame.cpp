#include "stdafx.h"
#include "cvui.h"
#include "CAsteroidGame.h"
#include "CControl.h"
#include "CAsteroid.h"
#include "CGameObject.h"
#include "CShip.h"

CAsteroidGame::CAsteroidGame(cv::Size size, int comid)
{
	_Canvas_size = size;
	CControl.init_com(comid);
	_canvas = cv::Mat::zeros(_Canvas_size, CV_8UC3);
	cvui::init("Asteroids");
	
	_fire_button = false;
	_reset_button = false;
	
	_deltaT = 0;
	_fps_average = 0;
	astroid_cycle_count = 0;
	_score = 0;
}

CAsteroidGame::~CAsteroidGame()
{

	cv::destroyAllWindows();

}

void CAsteroidGame::update()
{
	if (_reset == true) {
		_missile_list.clear();
		_asteroid_list.clear();
		_fire_button = false;
		_reset_button = false;
		astroid_cycle_count = 0;
		_score = 0;
		_ship.set_lives(10);
		_ship.set_pos(cv::Point2f(1200 / 2, 800 / 2));
		_reset = false;
	}
	CControl.get_button(32, _fire_button);
	CControl.get_button(33, _reset_button);
	if (_fire_button){ //fire missile
		CMissile missile;
		missile.set_pos(_ship.get_pos());
		_missile_list.push_back(missile);
		std::cout << "Fire ! \n";
	}
	if (_reset_button) { //reset button
		std::cout << "Reset ! \n";
		_reset = true;
	}
	if (astroid_cycle_count > 10) { //create astroid 

		CAsteroid astro;
		_asteroid_list.push_back(astro);
		astroid_cycle_count = 0;
		//std::cout << _ship.get_pos().x << "x\n" << _ship.get_pos().y << "y\n";

	}
	astroid_cycle_count++;
	CControl.get_analog(0, _analog_value.x);
	CControl.get_analog(1, _analog_value.y);
	int _draw_speed = 3;
	int _L_threshold_x = 1850;
	int _L_threshold_y = 1900;
	int _U_threshold_x = 2100;
	int _U_threshold_y = 2000;
	float vel_x = 0;
	float vel_y = 0;
	if (_analog_value.x < _L_threshold_x) //left
	{
		vel_x = vel_x - (abs(_analog_value.x - _L_threshold_x) / _draw_speed);
	}
	if (_analog_value.x > _U_threshold_x) //right
	{
		vel_x = vel_x + (abs(_analog_value.x - _U_threshold_x) / _draw_speed);
	}

	if (_analog_value.y > _U_threshold_y)//up
	{
		vel_y = vel_y - (abs(_analog_value.y - _U_threshold_y) / _draw_speed);
	}
	if (_analog_value.y < _L_threshold_y)//down
	{
		vel_y = vel_y + (abs(_analog_value.y - _L_threshold_x) / _draw_speed);
	}

	_ship.set_vel(cv::Point2f(vel_x, vel_y)); // set ship velocity

	_ship.move(_deltaT); //send delta time to ship

	if (_ship.get_pos().x - _ship.get_radius() < 0) { //check if ship is past left border
		_ship.set_pos(cv::Point2f(_ship.get_radius(), _ship.get_pos().y));
	}
	if (_ship.get_pos().x + _ship.get_radius() > _Canvas_size.width) { //check if ship is past right border
		_ship.set_pos(cv::Point2f(_Canvas_size.width - _ship.get_radius(), _ship.get_pos().y));
	}
	
	if (_ship.get_pos().y - _ship.get_radius() < 0) { //check if ship is past top border
		_ship.set_pos(cv::Point2f(_ship.get_pos().x, _ship.get_radius()));
	}
	if (_ship.get_pos().y + _ship.get_radius() > _Canvas_size.height) {//check if ship is past bottom border
		_ship.set_pos(cv::Point2f(_ship.get_pos().x, _Canvas_size.height - _ship.get_radius()));
	}
	for (int index = 0; index < _asteroid_list.size(); index++) { // Move Astroids
		_asteroid_list[index].move(_deltaT);
	}
	for (int index = 0; index < _missile_list.size(); index++) { // Move Missiles
		_missile_list[index].move(_deltaT);
	}
	for (int index_asteroid = 0; index_asteroid < _asteroid_list.size(); index_asteroid++) { // check for missile collisions on asteroid
		for (int index_missile = 0; index_missile < _missile_list.size(); index_missile++) {
			if (_missile_list[index_missile].collide(_asteroid_list[index_asteroid])) {
				_score += 10;
			}
		}
	}
	for (int index = 0; index < _asteroid_list.size(); index++) { // check for astroid collision on wall
		_asteroid_list[index].collide_wall(_Canvas_size);
	}
	for (int index = 0; index < _missile_list.size(); index++) { // check for missile collision on wall
		_missile_list[index].collide_wall(_Canvas_size);
	}
	for (int index = 0; index < _asteroid_list.size(); index++) { // check for asteroid collisions on ship
		_ship.collide(_asteroid_list[index]);
	}
	for (int index = 0; index < _asteroid_list.size(); index++) { // Check astroid lives then remove
		if (_asteroid_list[index].get_lives() <= 0) {
			_asteroid_list.erase(_asteroid_list.begin() + index);
		}
	}
	for (int index = 0; index < _missile_list.size(); index++) { // Check missile lives then remove
		if (_missile_list[index].get_lives() <= 0) {
			_missile_list.erase(_missile_list.begin() + index);
		}
	}	
}

void CAsteroidGame::draw()
{
	if (_ship.get_lives() <= 0) {
		cvui::printf(_canvas, 10, 10, 1.2, 0xFFFFFF, "You ran out of lives... Your score is %d", _score);
		cvui::printf(_canvas, 10, 45, 1.2, 0xFFFFFF, "Press reset button to play again !! ");
	}
	else 
	{
		_ship.draw(_canvas, cv::Scalar(255,0,0));
		for (int index = 0; index != _asteroid_list.size(); index++) { // Draw Astroids
			_asteroid_list[index].draw(_canvas, cv::Scalar(100, 100, 100));
		}
		for (int index = 0; index != _missile_list.size(); index++) { // Draw Missiles
			_missile_list[index].draw(_canvas, cv::Scalar(0, 0, 255));
		}
		cvui::printf(_canvas, 10, 10, 1.2, 0xFFFFFF, "FPS: %.2f     Score: %d     Lives: %d     Missiles: %d", _fps_average, _score, _ship.get_lives(), _missile_list.size());
		cv::imshow("Asteroids", _canvas);
	}
	cv::imshow("Asteroids", _canvas);
	_canvas = cv::Mat::zeros(_Canvas_size, CV_8UC3);
}

void CAsteroidGame::run()
{
	while (cv::waitKey(1) != 'q' && _exit != true ) {
		auto calc_start = std::chrono::steady_clock::now();
		auto end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(33); // 0.033 seconds = 33.333 milli = 33333.3 us
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
		update();
		draw();
		std::this_thread::sleep_until(end_time);
		auto calc_end = std::chrono::steady_clock::now();
		std::chrono::duration<double, std::milli> calc_elapsed = (calc_end - calc_start);
		_deltaT = (calc_elapsed.count() / 1000);
	}
}
