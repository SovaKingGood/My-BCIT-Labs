#include "stdafx.h"
#include "CGameObject.h"
#include "CAsteroidGame.h"  



void CGameObject::move(float time)
{
    _position = _position + _velocity * time;


}

bool CGameObject::collide(CGameObject& obj)
{
    
    if (get_pos().inside(cv::Rect2f(obj._position.x - obj._radius -_radius, obj._position.y - obj._radius - _radius, 2*obj._radius + 2*_radius, 2*obj._radius + 2*_radius))) {
    
        hit();
        obj.hit();
        return true;
    }
    return false;
}

bool CGameObject::collide_wall(cv::Size board)
{
    if (get_pos().x + get_radius() < 0) { //check if ship is past left border
        hit();
        return true;
    }
    if (get_pos().x - get_radius() > board.width) { //check if ship is past right border
        hit();
        return true;
    }

    if ( (get_pos().y + get_radius() < 0) && (get_vel().y < 0) ) { //check if ship is past top border
        hit();
        return true;
    }
    if (get_pos().y - get_radius() > board.height) {//check if ship is past bottom border
        hit();
        return true;
    }

    return false;
}

void CGameObject::hit()
{
    _lives--;
}

void CGameObject::draw(cv::Mat& im, cv::Scalar color)
{
    cv::circle(im, _position, _radius, color, -1);


}
