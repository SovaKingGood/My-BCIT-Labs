#pragma once
#include "CControl.h"
#include "CBase4618.h"
#include "stdafx.h"

/**
* @brief Used to play Pong
*
* This class is used to play pong against an AI. First to a score of 5 wins!
* The Tiva C Series Launch Pad joystick is used to control the paddle.
*
* @author Brennan Pinette
* @date Feb 24, 2023
* */

class CPong : public CBase4618 {

public:

	/**
	* @brief Initiates variables and canvas for Pong
	* 
	* @param Size of canvas
	* @param Serial port number
	*/
	CPong(cv::Size size, int comid);
	/**
* @brief Destructor closes the pong window
*
*/
	~CPong();

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
	* @brief Initiate update and draw thread and exit when q for exit flag is true
	*
	* */
	void run();

	/**
	* @brief Loop update() until _thread_exit flag is true
	*
	* @param ptr Address of object
	* 
	**/

	static void update_thread(CPong* ptr);

	/**
* @brief Loop draw() until _thread_exit flag is true
*
* @param ptr Address of object
*
**/
	static void draw_thread(CPong* ptr);
	 
	bool _thread_exit; ///< Thread exit flag
	cv::Point gui; ///< GUI x and y position

	int _radius; ///<Radius of pong
	int _score_p1; ///<score of human
	int _score_p2; ///<score of AI
	bool _game_over; ///<game over flag

	cv::Point _Paddle_Position_p1; ///< X and Y position of human paddle
	cv::Point _Paddle_Position_p1_last; ///< X and Y position of human paddle in the last frame

	cv::Point _Paddle_Position_p2;///< X and Y position of AI paddle
	cv::Point _Paddle_Position_p2_last;///< X and Y position of AI paddle in the last frame



	cv::Scalar color; ///< Color 
	cv::Size _Canvas_size; ///<Canvas Size

	cv::Point _Position; ///< Position of game object
	cv::Point _Velocity; ///< Velocity of game object

	int _speed_multiplier; ///< Speed adjuster (smaller = faster)

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