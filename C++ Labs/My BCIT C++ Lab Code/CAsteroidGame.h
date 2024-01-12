#pragma once
#include "CControlPi.h"
#include "CBase4618.h"
#include "CAsteroid.h"
#include "CMissile.h"
#include "CShip.h"
#include "CGameObject.h"




class CAsteroidGame : public CBase4618 {

protected:

	std::vector<CMissile> _missile_list;
	std::vector<CAsteroid> _asteroid_list;

	int _fire_button;
	int _reset_button;

	CShip _ship;
	int astroid_cycle_count;
	int _score;

public:

	CAsteroidGame(cv::Size size, int comid);
	~CAsteroidGame();

	/**
	* @brief Updates variables and perform calculations
	*/
	void update();

	/**
	* @brief Draw GUI and display
	*
	* */
	void draw();


	/**
	* @brief Loop run and draw until q for exit flag is set
	*
	* */
	void run();

	cv::Scalar color; ///< Color
	cv::Size _Canvas_size; ///<Canvas Size




	float _fps_current; ///< The current frames per second
	float _deltaT; ///< The time taken for one cycle of code to complete in seconds
	float _deltaT_milli; ///< The time taken for one cycle of code to complete in milliseconds
	float _fps_average; ///< The average frames per second

	std::vector<float> _fps_vector; ///< Framers per second vector for calculating average

	int _analog_x; ///< Analog X raw value

	int _analog_y;///< Analog y raw value

	bool _exit; // Exit Program flag. 1 = Exit

	bool _reset; ///< Reset Sketch flag. Reset = 1

	int _reset_button_state; ///< State of reset button


};
