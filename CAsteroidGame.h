#pragma once
#include "CControl.h"
#include "CBase4618.h"
#include "CAsteroid.h"
#include "CMissile.h"
#include "CShip.h"
#include "CGameObject.h"
#include "stdafx.h"


/**
* @brief Used play Asteroids !
*
* 
* This class requirs the class CControl along with the Tiva C Series Launch Pad to play.
*
* @author Brennan Pinette
* @date Feb 28, 2023
* */
class CAsteroidGame : public CBase4618 {

protected:

	std::vector<CMissile> _missile_list; ///< Missile Object Vector
	std::vector<CAsteroid> _asteroid_list; ///< Asteroid Object Vector

	int _fire_button; ///< Fire button state
	int _reset_button;///< Reset button state

	CShip _ship; ///< Ship object
	int astroid_cycle_count; ///< This variable determines how many frames are required to pass before another asteroid is summoned.
	int _score; ///< Score of the game. +10 for every asteroid destroyed
	
public:

	/**
	*@brief Initiates variables and canvas for game
	* 
	* @param size Size of canvas
	* 
	* @param comid Serial port number
	**/
	CAsteroidGame(cv::Size size, int comid);
	/**
	*@brief Close window
	* 
	* 
	**/
	~CAsteroidGame();

	/**
	* @brief Updates variables and perform calculations
	*/
	void update();

	/**
	* @brief Draw objects and display game info
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

	cv::Point2i _analog_value; ///< Analog value

	bool _exit; // Exit Program flag. 1 = Exit

	bool _reset; ///< Reset Sketch flag. Reset = 1

	int _reset_button_state; ///< State of reset button
};