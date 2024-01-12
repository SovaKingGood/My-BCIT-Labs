
#include "CMissile.h"

CMissile::CMissile() {

	set_lives(1);
	_radius = 2;
	_velocity = cv::Point2f(0, -500);

}
