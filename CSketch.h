#pragma once
#include "CControl.h"
#include "CBase4618.h"
#include "stdafx.h"

/**
* @brief Used to play Etch-A-Sketch !
*
* This class is used to draw and update the canvas for Etch-A-Sketch
* The Tiva C Series Launch Pad joystick is used to draw, change color, and reset the canvas.
*
* @author Brennan Pinette
* @date Feb 16, 2023
* */

class CSketch : public CBase4618 {



public:



	

/**
* @brief Used to initialize object comport and canvas window 
* 
* @param size Size of window (Length x Width)
* @param comid serial port communication number
* */
	CSketch(cv::Size size, int comid);

	/**
	* @brief Destructor closes the Etch-A-Sketch window.
	* 
	* */
	~CSketch();

	/**
	* @brief Updates variables and perform calculations
	* 
	* */
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

	int _L_threshold_x; ///< Lower threshold of horizontal deadzone 

	int _L_threshold_y;///< Lower threshold of vertical deadzone 

	int _U_threshold_x; ///< Upper threshold of horizontal deadzone 

	int _U_threshold_y; ///< Upper threshold of vertical deadzone 

	int _draw_speed; ///< Variable that is inversely proportional to the drawing speed

	int color_select; ///< Color of Sketch [ Blue = 1, Green = 2, Red = 3 ]

	int _color_button_state; //< State of color button

	int draw_x;///<X position of draw

	int draw_y; ///<Y position of draw

	cv::Point drawer; ///< Sketch draw x and y position
	cv::Point drawer_previous; ///< Sketch draw x and y position from last frame



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
	cv::Point gui; ///< GUI x and y position
};
