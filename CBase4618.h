#pragma once
#include "CControl.h"
#include "stdafx.h"

class CBase4618 {

public:

	virtual void draw() = 0;
	virtual void update() = 0;
	/**
	* @brief Runs update and draw in a loop until exit program
	* 
	**/
	void run();


	CControl CControl; ///< Initiate CControl object named CControl
	cv::Mat _canvas; ///< Initiate Mat Object named _canvas
	


};
