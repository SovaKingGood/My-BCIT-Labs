#include "stdafx.h"
#include "CShip.h"
#include "CGameObject.h"


CShip::CShip() {

	set_lives(10);
	_radius = 30;
	_position = cv::Point2f(1200/2, 800/2);
	_velocity = cv::Point2f(0, 0);



}

