#pragma once
//#include "Serial.h"
/**
* @brief Used to communicate with embedded systems.
*
* This class is used to send and recieve data from
* Tiva C Series Launch Pad.
*
* @author Brennan Pinette
* @date Jan 26, 2023
* */
class CControlPi {

private:


	//Serial _com; ///< Initalize Serial Communication Object for CControl.


public:

	CControlPi();
	~CControlPi();







	/** @brief Initalizes the embedded system at the specified COM port.
	*
	* @param comport The COM port number.
	* @return nothing to return
	*
	* */
	void init_com(int comport);

	/** @brief Receive data from embedded system.
	*
	* @param type 0 = Digital | 1 = Analog | 2 = Servo
	* @param channel channel #
	* @param result Data received
	* @return True if received succesfully. False if failed.
	*
	* */
	bool get_data(int type, int channel, int& result);
	bool get_data(int type, int channel, bool& result);

	/** @brief Sends data to embedded system.
	*
	* @param type 0 = Digital | 1 = Analog | 2 = Servo
	* @param channel Channel number
	* @param value Value to send
	*
	* @return True if sent succesfully. False if failed.
	*
	* */
	bool set_data(int type, int channel, int val);

	/** @brief Gets analog data from joy-stick as percent
	*
	* @param channel Channel number ( 0 for horizontal, 1 for vertical)
	* @param result Percentage of joy-stick position
	*
	*
	* @return True if received succesfully. False if failed.
	*
	* */
	bool get_analog(int channel, int& result);

	/** @brief Gets button state
	*
	* @param channel Channel number ( X=0 , Y=1)
	* @param result Button State
	*
	*
	* @return True if received succesfully. False if failed.
	*
	* */
	bool get_button(int channel, int& result);
	bool get_button(int channel, bool& result);




	int debounce[200]; ///<Store button(s) state
	int debounce_last[200];///<Store button(s) previous state
	double start_time[200]; ///<Store start timer for button(s)
};
