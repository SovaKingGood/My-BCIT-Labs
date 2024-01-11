#include "stdafx.h"
#include "CMissile.h"

CMissile::CMissile() {

	set_lives(1);
	_radius = 4;
	_velocity = cv::Point2f(0, -700);

}