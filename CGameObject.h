#pragma once
#include "stdafx.h"
/**
* @brief Stores game variables. 
*
*
* This class holds variables such as position, velocity, radius, and lives.
* 
* 
*
* @author Brennan Pinette
* @date Feb 28, 2023
* */
class CGameObject {
protected:
	cv::Point2f _position; ///< Float of position value
	cv::Point2f _velocity; ///< Float of velocity value
	
	int _radius; ///< Radius value
	int _lives; ////< Current lives
public:
	/**
*@brief Move object based on current velocity, position, and time since last frame.
*
* @param time Time since last frame
*
* 
**/
	void move(float time);
	/**
*@brief Check if there is a collision between two objects
*
* @param obj Object to check if there is a collision
*
*return Return true if collision was detected
**/
	bool collide(CGameObject& obj);
	/**
*@brief Check if object has left the canvas
* For collisions on the top of the screen, velocity is also checked such that asteroids do not count as collisions but missiles do. 
*
* @param board Size of canvas
*
*@return Return true of object has left the screen
**/
	bool collide_wall(cv::Size board);
	/**
*@brief Reduce life by 1
*
* 
*
*
**/
	void hit();
	/**
*@brief Return current lives
*
*@return Lives
*
*
**/
	int get_lives() { return _lives; };
	/**
*@brief Set current lives
*
* @param lives Value to set lives to
*
*
**/
	void set_lives(int lives) { _lives = lives; }
	/**
*@brief Set position 
*
* @param pos Position to set object to
*
*
**/
	void set_pos(cv::Point2f pos) { _position = pos; }
	/**
*@brief Set Velocity
*
* @param vel Velocity to set object to
*
*
**/
	void set_vel(cv::Point2f vel) { _velocity = vel; }
	/**
*@brief Get the radius value
*
* @return Radius
*
*
**/
	int get_radius() { return _radius; };
	/**
*@brief Get current position
*
* @return Position
*
*
**/
	cv::Point2f get_pos() { return _position;  }
	/**
*@brief Get current Velocity
*
* @return Velocity
*
*
**/
	cv::Point2f get_vel() { return _velocity; }
	/**
*@brief Draw object on canvas
*
* @param im The canvas
* @parma color The color of the object
*
*
**/
	void draw(cv::Mat& im, cv::Scalar color);
	
};
