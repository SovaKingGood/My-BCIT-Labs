#include "stdafx.h"
#include "CAsteroid.h"


CAsteroid::CAsteroid() {

	//random radius between 10 and 20
	_radius = rand() % 21 + 10;

	// random x position between 0 and 1199
	// y position -50
	_position = cv::Point2f(rand() % 1200, -50);

	//random x velocity between 1 and 2 in random direction
	//random y velocity between 1 and 5 going down
	_velocity = cv::Point2f( ( rand() % 50 + 40 ) * ( -1 * rand() % 2 ) , ( rand() % 100 + 50 ) );

	set_lives(1);




}